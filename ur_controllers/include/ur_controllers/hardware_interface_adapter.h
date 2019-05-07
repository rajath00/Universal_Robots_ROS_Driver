// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

// -- BEGIN LICENSE BLOCK ----------------------------------------------
// -- END LICENSE BLOCK ------------------------------------------------

//----------------------------------------------------------------------
/*!\file
 *
 * \author  Felix Mauch mauch@fzi.de
 * \date    2019-04-18
 *
 */
//----------------------------------------------------------------------

#ifndef UR_CONTROLLERS_HARDWARE_INTERFACE_ADAPTER_H_INCLUDED
#define UR_CONTROLLERS_HARDWARE_INTERFACE_ADAPTER_H_INCLUDED

#include <joint_trajectory_controller/hardware_interface_adapter.h>
#include "ur_controllers/scaled_joint_command_interface.h"

/**
 * \brief Adapter for a position-controlled hardware interface. Forwards desired positions as commands.
 *
 * The following is an example configuration of a controller that uses this adapter.
 * \code
 * head_controller:
 *   type: "position_controllers/ScaledJointTrajectoryController"
 *   joints:
 *     - head_1_joint
 *     - head_2_joint
 *
 *   constraints:
 *     goal_time: 0.6
 *     stopped_velocity_tolerance: 0.02
 *     head_1_joint: {trajectory: 0.05, goal: 0.02}
 *     head_2_joint: {trajectory: 0.05, goal: 0.02}
 *   stop_trajectory_duration: 0.5
 *   state_publish_rate:  25
 * \endcode
 */
template <class State>
class HardwareInterfaceAdapter<ur_controllers::ScaledPositionJointInterface, State>
{
public:
  HardwareInterfaceAdapter() : joint_handles_ptr_(0)
  {
  }

  bool init(std::vector<ur_controllers::ScaledJointHandle>& joint_handles, ros::NodeHandle& /*controller_nh*/)
  {
    // Store pointer to joint handles
    joint_handles_ptr_ = &joint_handles;

    return true;
  }

  void starting(const ros::Time& /*time*/)
  {
    if (!joint_handles_ptr_)
    {
      return;
    }

    // Semantic zero for commands
    for (auto& jh : *joint_handles_ptr_)
    {
      jh.setCommand(jh.getPosition());
    }
  }

  void stopping(const ros::Time& /*time*/)
  {
  }

  void updateCommand(const ros::Time& /*time*/, const ros::Duration& /*period*/, const State& desired_state,
                     const State& /*state_error*/)
  {
    // Forward desired position to command
    const unsigned int n_joints = joint_handles_ptr_->size();
    for (unsigned int i = 0; i < n_joints; ++i)
    {
      (*joint_handles_ptr_)[i].setCommand(desired_state.position[i]);
    }
  }

private:
  std::vector<ur_controllers::ScaledJointHandle>* joint_handles_ptr_;
};

#endif  // ifndef UR_CONTROLLERS_HARDWARE_INTERFACE_ADAPTER_H_INCLUDED