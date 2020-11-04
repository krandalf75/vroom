/*

This file is part of VROOM.

Copyright (c) 2015-2019, Julien Coupey.
All rights reserved (see LICENSE).

*/

#include <numeric>

#include <glpk.h>

#include "algorithms/validation/choose_invalid.h"

namespace vroom {
namespace validation {

Route choose_invalid_route(const Input& input,
                           unsigned vehicle_rank,
                           const std::vector<InputStep>& steps,
                           std::unordered_set<Index>& unassigned_ranks) {
  const auto& m = input.get_matrix();
  const unsigned n = steps.size();
  const auto& v = input.vehicles[vehicle_rank];

  constexpr double M = 10.0;

  // Total number of time windows.
  const unsigned K =
    std::accumulate(steps.begin(),
                    steps.end(),
                    0,
                    [&](auto sum, const auto& step) {
                      return sum + input.jobs[step.rank].tws.size();
                    });

  // Create problem.
  glp_prob* lp;
  lp = glp_create_prob();
  glp_set_prob_name(lp, "choose_ETA");
  glp_set_obj_dir(lp, GLP_MIN);

  // Define constraints and remember number of non-zero values in the
  // matrix.
  const unsigned nb_constraints = 4 * n + 3;
  const unsigned nb_non_zero = 2 * (3 * n + 3) + 3 * K;

  glp_add_rows(lp, nb_constraints);

  unsigned current_row = 1;

  // Precedence constraints.
  double first = 0;
  if (v.has_start()) {
    // Take into account time from start point to first job.
    first = m[v.start.value().index()][input.jobs[steps.front().rank].index()];
  }
  glp_set_row_name(lp, current_row, "P0");
  glp_set_row_bnds(lp, current_row, GLP_LO, first, 0.0);
  ++current_row;

  for (unsigned i = 0; i < n - 1; ++i) {
    double dist = m[input.jobs[steps[i].rank].index()]
                   [input.jobs[steps[i + 1].rank].index()];
    double service = input.jobs[steps[i].rank].service;
    auto name = "P" + std::to_string(i + 1);
    glp_set_row_name(lp, current_row, name.c_str());
    glp_set_row_bnds(lp, current_row, GLP_LO, dist + service, 0.0);
    ++current_row;
  }

  double last = 0;
  if (v.has_end()) {
    // Take into account time from last job arrival to end point.
    last = m[input.jobs[steps.back().rank].index()][v.end.value().index()] +
           input.jobs[steps.back().rank].service;
  }
  glp_set_row_name(lp, current_row, ("P" + std::to_string(n)).c_str());
  glp_set_row_bnds(lp, current_row, GLP_LO, last, 0.0);
  ++current_row;

  assert(current_row == n + 2);

  // Vehicle TW start violation constraint.
  double lb = v.tw.start;
  glp_set_row_name(lp, current_row, "L0");
  glp_set_row_bnds(lp, current_row, GLP_LO, lb, 0.0);
  ++current_row;

  // Lead time ("earliest violation") constraints.
  for (unsigned i = 0; i < n; ++i) {
    auto name = "L" + std::to_string(i + 1);
    glp_set_row_name(lp, current_row, name.c_str());
    glp_set_row_bnds(lp, current_row, GLP_LO, 0.0, 0.0);
    ++current_row;
  }
  assert(current_row == 2 * n + 3);

  // Delay ("latest violation") constraints.
  for (unsigned i = 0; i < n; ++i) {
    auto name = "D" + std::to_string(i + 1);
    glp_set_row_name(lp, current_row, name.c_str());
    glp_set_row_bnds(lp, current_row, GLP_UP, 0.0, 0.0);
    ++current_row;
  }

  // Vehicle TW end violation constraint.
  double ub = v.tw.end;
  auto name = "L" + std::to_string(n + 1);
  glp_set_row_name(lp, current_row, name.c_str());
  glp_set_row_bnds(lp, current_row, GLP_UP, 0.0, ub);
  ++current_row;

  assert(current_row == 3 * n + 4);

  // Binary variable decision constraints.
  for (unsigned i = 1; i <= n; ++i) {
    auto name = "s" + std::to_string(i);
    glp_set_row_name(lp, current_row, name.c_str());
    glp_set_row_bnds(lp, current_row, GLP_FX, 1.0, 1.0);
    ++current_row;
  }
  assert(current_row == nb_constraints + 1);

  // Set variables and coefficients.
  const unsigned nb_var = 2 * n + 4 + K;
  glp_add_cols(lp, nb_var);

  unsigned current_col = 1;
  // Wanabee ETA.
  for (unsigned i = 0; i <= n + 1; ++i) {
    auto name = "t" + std::to_string(i);
    glp_set_col_name(lp, current_col, name.c_str());
    glp_set_col_bnds(lp, current_col, GLP_LO, 0.0, 0.0);
    ++current_col;
  }

  // Set makespan in objective.
  glp_set_obj_coef(lp, 1, -1.0);
  glp_set_obj_coef(lp, n + 2, 1.0);

  // Define variables for measure of TW violation and set in
  // objective.
  for (unsigned i = 0; i <= n + 1; ++i) {
    auto name = "Y" + std::to_string(i);
    glp_set_col_name(lp, current_col, name.c_str());
    glp_set_col_bnds(lp, current_col, GLP_LO, 0.0, 0.0);
    glp_set_obj_coef(lp, current_col, M);
    ++current_col;
  }
  assert(current_col == 2 * n + 5);

  // Binary variables for job time window choice.
  for (unsigned i = 0; i < n; ++i) {
    for (unsigned k = 0; k < input.jobs[steps[i].rank].tws.size(); ++k) {
      auto name = "X" + std::to_string(i + 1) + "_" + std::to_string(k);
      glp_set_col_name(lp, current_col, name.c_str());
      glp_set_col_kind(lp, current_col, GLP_BV);
      ++current_col;
    }
  }
  assert(current_col == nb_var + 1);

  // Define non-zero elements in matrix.
  int* ia = new int[1 + nb_non_zero];
  int* ja = new int[1 + nb_non_zero];
  double* ar = new double[1 + nb_non_zero];

  unsigned r = 1;
  // Coefficients for precedence constraints.
  for (unsigned i = 1; i <= n + 1; ++i) {
    // a[i,i] = -1
    ia[r] = i;
    ja[r] = i;
    ar[r] = -1;
    ++r;

    // a[i,i + 1] = 1
    ia[r] = i;
    ja[r] = i + 1;
    ar[r] = 1;
    ++r;
  }

  unsigned constraint_rank = n + 2;

  // Coefficients for L0 constraint.
  // a[constraint_rank, 1] = 1
  ia[r] = constraint_rank;
  ja[r] = 1;
  ar[r] = 1;
  ++r;

  // a[constraint_rank, n + 3] = 1
  ia[r] = constraint_rank;
  ja[r] = n + 3;
  ar[r] = 1;
  ++r;
  ++constraint_rank;

  // Coefficients other L_i constraints. current_X_rank is the rank
  // for binaries that describe the time window choices.
  unsigned current_X_rank = 2 * n + 4 + 1;

  for (unsigned i = 2; i <= n + 1; ++i) {
    // a[constraint_rank, i] = 1
    ia[r] = constraint_rank;
    ja[r] = i;
    ar[r] = 1;
    ++r;

    // a[constraint_rank, n + 2 + i] = 1
    ia[r] = constraint_rank;
    ja[r] = n + 2 + i;
    ar[r] = 1;
    ++r;

    const auto& job = input.jobs[steps[i - 2].rank];
    for (const auto& tw : job.tws) {
      // a[constraint_rank, current_X_rank] = - earliest_date for k-th TW of job
      ia[r] = constraint_rank;
      ja[r] = current_X_rank;
      ar[r] = -static_cast<double>(tw.start);
      ++r;

      ++current_X_rank;
    }

    ++constraint_rank;
  }
  assert(current_X_rank == 2 * n + 4 + K + 1);
  assert(constraint_rank == 2 * n + 3);

  // Coefficients for D_i constraints.
  current_X_rank = 2 * n + 4 + 1;

  for (unsigned i = 2; i <= n + 1; ++i) {
    // a[constraint_rank, i] = 1
    ia[r] = constraint_rank;
    ja[r] = i;
    ar[r] = 1;
    ++r;

    // a[constraint_rank, n + 2 + i] = -1
    ia[r] = constraint_rank;
    ja[r] = n + 2 + i;
    ar[r] = -1;
    ++r;

    const auto& job = input.jobs[steps[i - 2].rank];
    for (const auto& tw : job.tws) {
      // a[constraint_rank, current_X_rank] = - latest_date for k-th TW of job
      ia[r] = constraint_rank;
      ja[r] = current_X_rank;
      ar[r] = -static_cast<double>(tw.end);
      ++r;

      ++current_X_rank;
    }

    ++constraint_rank;
  }
  assert(current_X_rank == 2 * n + 4 + K + 1);

  // Coefficients D_{n + 1} constraint.
  // a[constraint_rank, n + 2] = 1
  ia[r] = constraint_rank;
  ja[r] = n + 2;
  ar[r] = 1;
  ++r;

  // a[constraint_rank, 2 * n + 4] = -1
  ia[r] = constraint_rank;
  ja[r] = 2 * n + 4;
  ar[r] = -1;
  ++r;
  ++constraint_rank;

  assert(constraint_rank == 3 * n + 4);

  // Decision constraints S_i for binary variables.
  current_X_rank = 2 * n + 4 + 1;

  for (unsigned i = 0; i < n; ++i) {
    const auto& job = input.jobs[steps[i].rank];
    for (unsigned k = 0; k < job.tws.size(); ++k) {
      // a[constraint_rank, current_X_rank] = 1
      ia[r] = constraint_rank;
      ja[r] = current_X_rank;
      ar[r] = 1;
      ++r;

      ++current_X_rank;
    }
    ++constraint_rank;
  }
  assert(current_X_rank == 2 * n + 4 + K + 1);
  assert(constraint_rank == nb_constraints + 1);

  assert(r == nb_non_zero + 1);

  glp_load_matrix(lp, nb_non_zero, ia, ja, ar);

  delete[] ia;
  delete[] ja;
  delete[] ar;

  // Solve.
  glp_term_out(GLP_OFF);
  glp_iocp parm;
  glp_init_iocp(&parm);
  parm.presolve = GLP_ON;
  glp_intopt(lp, &parm);

  // glp_print_mip(lp, "mip.sol");

  // Get output.
  auto v_start = glp_mip_col_val(lp, 1);
  auto v_end = glp_mip_col_val(lp, n + 2);

  std::vector<Duration> job_ETA;
  for (unsigned i = 2; i <= n + 1; ++i) {
    job_ETA.push_back(glp_mip_col_val(lp, i));
  }

  // Populate vector storing picked time window ranks.
  current_X_rank = 2 * n + 4 + 1;
  std::vector<Index> job_tw_ranks;

  for (unsigned i = 0; i < n; ++i) {
    const auto& job = input.jobs[steps[i].rank];
    for (unsigned k = 0; k < job.tws.size(); ++k) {
      auto val = glp_mip_col_val(lp, current_X_rank);
      if (val == 1) {
        job_tw_ranks.push_back(k);
      }

      ++current_X_rank;
    }
    assert(job_tw_ranks.size() == i + 1);
  }
  assert(current_X_rank == 2 * n + 4 + K + 1);

  glp_delete_prob(lp);
  glp_free_env();

  // Generate route.
  Cost duration = 0;
  Duration service = 0;
  Duration forward_wt = 0;
  Priority priority = 0;
  Amount sum_pickups(input.zero_amount());
  Amount sum_deliveries(input.zero_amount());
  Duration lead_time = 0;
  Duration delay = 0;

  // Startup load is the sum of deliveries for jobs.
  Amount current_load(input.zero_amount());
  for (const auto& step : steps) {
    const auto& job = input.jobs[step.rank];
    if (job.type == JOB_TYPE::SINGLE) {
      current_load += job.delivery;
    }
  }

  // TODO check for LOAD violation at route level in case there is no
  // start.

  // Used for precedence violations.
  std::unordered_set<Index> expected_delivery_ranks;
  std::unordered_set<Index> delivery_first_ranks;
  std::unordered_map<Index, Index> delivery_to_pickup_step_rank;

  std::vector<Step> sol_steps;

  Duration next_arrival;

  if (v.has_start()) {
    sol_steps.emplace_back(STEP_TYPE::START, v.start.value(), current_load);
    sol_steps.back().duration = 0;
    sol_steps.back().arrival = v_start;
    if (v_start < v.tw.start) {
      sol_steps.back().violations.insert(VIOLATION::LEAD_TIME);
      Duration lt = v.tw.start - v_start;
      sol_steps.back().lead_time = lt;
      lead_time += lt;
    }

    if (!(current_load <= v.capacity)) {
      sol_steps.back().violations.insert(VIOLATION::LOAD);
    }

    auto travel =
      m[v.start.value().index()][input.jobs[steps.front().rank].index()];
    duration += travel;
    next_arrival = v_start + travel;
  } else {
    next_arrival = job_ETA.front();
  }

  auto& first_job = input.jobs[steps.front().rank];
  service += first_job.service;
  priority += first_job.priority;

  current_load += first_job.pickup;
  current_load -= first_job.delivery;
  sum_pickups += first_job.pickup;
  sum_deliveries += first_job.delivery;

  sol_steps.emplace_back(first_job, current_load);
  auto& first_step = sol_steps.back();
  first_step.duration = duration;
  auto service_start = job_ETA.front();
  assert(next_arrival <= service_start);

  first_step.arrival = next_arrival;

  Duration wt = service_start - next_arrival;
  first_step.waiting_time = wt;
  forward_wt += wt;

  // Handle violations.
  auto first_tw_rank = job_tw_ranks.front();
  if (service_start < first_job.tws[first_tw_rank].start) {
    first_step.violations.insert(VIOLATION::LEAD_TIME);
    Duration lt = first_job.tws[first_tw_rank].start - service_start;
    first_step.lead_time = lt;
    lead_time += lt;
  }
  if (first_job.tws[first_tw_rank].end < service_start) {
    first_step.violations.insert(VIOLATION::DELAY);
    Duration dl = service_start - first_job.tws[first_tw_rank].end;
    first_step.delay = dl;
    delay += dl;
  }
  if (!(current_load <= v.capacity)) {
    first_step.violations.insert(VIOLATION::LOAD);
  }
  if (first_job.type == JOB_TYPE::PICKUP) {
    expected_delivery_ranks.insert(steps.front().rank + 1);
    delivery_to_pickup_step_rank.emplace(steps.front().rank + 1, 0);
  }
  if (first_job.type == JOB_TYPE::DELIVERY) {
    first_step.violations.insert(VIOLATION::PRECEDENCE);
    delivery_first_ranks.insert(steps.front().rank);
  }

  unassigned_ranks.erase(steps.front().rank);

  for (std::size_t r = 0; r < steps.size() - 1; ++r) {
    const auto& previous_job = input.jobs[steps[r].rank];
    const auto next_job_rank = steps[r + 1].rank;
    const auto& next_job = input.jobs[next_job_rank];
    const auto& next_tw_rank = job_tw_ranks[r + 1];

    Duration travel = m[previous_job.index()][next_job.index()];
    duration += travel;
    next_arrival = service_start + previous_job.service + travel;

    service += next_job.service;
    priority += next_job.priority;

    current_load += next_job.pickup;
    current_load -= next_job.delivery;
    sum_pickups += next_job.pickup;
    sum_deliveries += next_job.delivery;

    sol_steps.emplace_back(next_job, current_load);
    auto& current = sol_steps.back();
    current.duration = duration;

    service_start = job_ETA[r + 1];
    assert(next_arrival <= service_start);

    current.arrival = next_arrival;
    Duration wt = service_start - next_arrival;
    current.waiting_time = wt;
    forward_wt += wt;

    // Handle violations.
    if (service_start < next_job.tws[next_tw_rank].start) {
      current.violations.insert(VIOLATION::LEAD_TIME);
      Duration lt = next_job.tws[next_tw_rank].start - service_start;
      current.lead_time = lt;
      lead_time += lt;
    }
    if (next_job.tws[next_tw_rank].end < service_start) {
      current.violations.insert(VIOLATION::DELAY);
      Duration dl = service_start - next_job.tws[next_tw_rank].end;
      current.delay = dl;
      delay += dl;
    }
    if (!(current_load <= v.capacity)) {
      current.violations.insert(VIOLATION::LOAD);
    }
    switch (next_job.type) {
    case JOB_TYPE::SINGLE:
      break;
    case JOB_TYPE::PICKUP:
      if (delivery_first_ranks.find(next_job_rank + 1) !=
          delivery_first_ranks.end()) {
        current.violations.insert(VIOLATION::PRECEDENCE);
      } else {
        expected_delivery_ranks.insert(next_job_rank + 1);
        delivery_to_pickup_step_rank.emplace(next_job_rank + 1,
                                             sol_steps.size() - 1);
      }
      break;
    case JOB_TYPE::DELIVERY:
      auto search = expected_delivery_ranks.find(next_job_rank);
      if (search == expected_delivery_ranks.end()) {
        current.violations.insert(VIOLATION::PRECEDENCE);
        delivery_first_ranks.insert(next_job_rank);
      } else {
        expected_delivery_ranks.erase(search);
      }
      break;
    }

    unassigned_ranks.erase(next_job_rank);
  }

  if (v.has_end()) {
    const auto& last_job = input.jobs[steps.back().rank];
    Duration travel = m[last_job.index()][v.end.value().index()];
    duration += travel;

    assert(service_start + last_job.service + travel == v_end);

    sol_steps.emplace_back(STEP_TYPE::END, v.end.value(), current_load);
    sol_steps.back().duration = duration;
    sol_steps.back().arrival = v_end;

    if (v.tw.end < v_end) {
      sol_steps.back().violations.insert(VIOLATION::DELAY);
      Duration dl = v_end - v.tw.end;
      sol_steps.back().delay = dl;
      delay += dl;
    }
    if (!(current_load <= v.capacity)) {
      sol_steps.back().violations.insert(VIOLATION::LOAD);
    }
  }

  // Precedence violations for pickups without a delivery.
  for (const auto d_rank : expected_delivery_ranks) {
    auto search = delivery_to_pickup_step_rank.find(d_rank);
    assert(search != delivery_to_pickup_step_rank.end());
    sol_steps[search->second].violations.insert(VIOLATION::PRECEDENCE);
  }

  return Route(v.id,
               std::move(sol_steps),
               duration,
               service,
               duration,
               forward_wt,
               priority,
               sum_deliveries,
               sum_pickups,
               v.description,
               lead_time,
               delay);
}

} // namespace validation
} // namespace vroom
