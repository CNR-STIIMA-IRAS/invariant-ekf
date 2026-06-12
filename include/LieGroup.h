/* ----------------------------------------------------------------------------
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2018, Ross Hartley
 * Copyright (c) 2026, Nicola Pedrocchi, CNR
 * All rights reserved.
 * See LICENSE for full BSD-3-Clause terms.
 * -------------------------------------------------------------------------- */

/**
 *  @file   LieGroup.h
 *  @author Ross Hartley
 *  @brief  Header file for various Lie Group functions 
 *  @date   September 25, 2018
 **/

#ifndef LIEGROUP_H
#define LIEGROUP_H 
#include <Eigen/Dense>
#include <iostream>

namespace inekf {

extern const double TOLERANCE;

Eigen::Matrix3d skew(const Eigen::Vector3d& v);
Eigen::Matrix3d Exp_SO3(const Eigen::Vector3d& w);
Eigen::MatrixXd Exp_SEK3(const Eigen::VectorXd& v);
Eigen::MatrixXd Adjoint_SEK3(const Eigen::MatrixXd& X);

} // end inekf namespace
#endif 
