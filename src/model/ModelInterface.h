/*
 * ModelInterface.h
 *
 *  Created on: Dec 13, 2016
 *      Author: Mikael Jorda
 */

#ifndef MODELINTERFACE_H_
#define MODELINTERFACE_H_

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <string>

namespace Model
{

enum ParserType {yml, urdf};

enum ModelType {rbdl};


//forward define
class ModelInternal;

class ModelInterface
{
public:

    /**
     * @brief creates a model interface object that contains a model of the robot, and functions to compute the jacobians, transformation matrices ...
     * @param path_to_model_file a path to the file containing the model of the robot (urdf and yml files supported)
     * @param model_type The type of the model we want to create (rbdl model supported)
     * @param parser the type of parser to use in function of the file type (urdf or yml)
     * @param verbose to display information about the model vreation in the terminal or not
     */
    ModelInterface (const std::string path_to_model_file,
                    Model::ModelType model_type,
                    Model::ParserType parser,
                    bool verbose = false);

    ~ModelInterface ();

    /**
     * @brief update the model. call after updating joint positions, velocities and acceleration.
     * This will compute all the transformation matrices and the mass matrix, gravity vector, and coriolis vector.
     * The other functions are accessors only.
     * The jacobian functions will compute the jacobians based on the robot configuration when updatedModel was last called
     */
    void updateModel();

protected :
    /**
     * @brief Gives the mass matrix for the last updated configuration
     * @param A matrix to which the mass matrix will be written. Size must be _dof x _dof
     */
    void massMatrix(Eigen::MatrixXd& A);

public :
    /**
     * @brief Gives the joint gravity torques vector for the last updated configuration
     * @param g Vector to which the joint gravity torques will be written
     * @param gravity the 3d gravity vector of the world in base frame
     */
    void gravityVector(Eigen::VectorXd& g,
                       const Eigen::Vector3d& gravity = Eigen::Vector3d(0,0,-9.8));

    /**
     * @brief Gives the joint coriolis and centrifugal forces for the last updated configuration
     * @param b Vector to which the joint coriolis and centrifugal forces will be written
     */
    void coriolisForce(Eigen::VectorXd& b);

    /**
     * @brief Full jacobian for point on link, relative to base (id=0) for the last updated configuration
     * @param J Matrix to which the jacobian will be written
     * @param link_name the name of the link where to compute the jacobian
     * @param pos_in_link the position of the point in the link where the jacobian is computed (in local link frame)
     */
    void J(Eigen::MatrixXd& J,
            const std::string& link_name,
            const Eigen::Vector3d& pos_in_link);

    /**
     * @brief Velocity jacobian for point on link, relative to base (id=0) for the last updated configuration
     * @param J Matrix to which the jacobian will be written
     * @param link_name the name of the link where to compute the jacobian
     * @param pos_in_link the position of the point in the link where the jacobian is computed (in local link frame)
     */
    void Jv(Eigen::MatrixXd& J,
            const std::string& link_name,
            const Eigen::Vector3d& pos_in_link);

    /**
     * @brief Angular velocity jacobian for link, relative to base (id=0) for the last updated configuration
     * @param J Matrix to which the jacobian will be written
     * @param link_name the name of the link where to compute the jacobian
     */
    void Jw(Eigen::MatrixXd& J,
            const std::string& link_name);

    /**
     * @brief transformation from base to link, in base coordinates, for the last updated configuration
     * @param T Transformation matrix to which the result is computed
     * @param link_name name of the link where to compute the transformation matrix
     */
    void transform(Eigen::Affine3d& T,
                   const std::string& link_name);

    /**
     * @brief Position from base to point in link, in base coordinates for the last updated configuration
     * @param pos Vector of position to which the result is written
     * @param link_name name of the link in which is the point where to compute the position
     * @param pos_in_link the position of the point in the link, in local link frame
     */
    void position(Eigen::Vector3d& pos,
                  const std::string& link_name,
                  const Eigen::Vector3d& pos_in_link);

    /**
     * @brief Velocity of point in link, in base coordinates for the last updated configuration
     * @param vel Vector of velocities to which the result is written
     * @param link_name name of the link in which is the point where to compute the velocity
     * @param pos_in_link the position of the point in the link, in local link frame
     */
    void linearVelocity(Eigen::Vector3d& vel,
                  const std::string& link_name,
                  const Eigen::Vector3d& pos_in_link);

    /**
     * @brief Acceleration of point in link, in base coordinates for the last updated configuration
     * @param accel Vector of accelerations to which the result is written
     * @param link_name name of the link in which is the point where to compute the acceleration
     * @param pos_in_link the position of the point in the link, in local link frame
     */
    void linearAcceleration(Eigen::Vector3d& accel,
                      const std::string& link_name,
                      const Eigen::Vector3d& pos_in_link);

    /**
     * @brief Rotation of a link with respect to base frame for the last updated configuration
     * @param rot Rotation matrix to which the result is written
     * @param link_name name of the link for which to compute the rotation
     */
    void rotation(Eigen::Matrix3d& rot,
                  const std::string& link_name);

    /**
     * @brief Angular velocity of a link with respect to base frame for the last updated configuration
     * @param avel Vector to which the result is written
     * @param link_name name of the link for which to compute the angular velocity
     */
    void angularVelocity(Eigen::Vector3d& avel,
                         const std::string& link_name);

    /**
     * @brief Angular acceleration of a link with respect to base frame for the last updated configuration
     * @param aaccel Vector to which the result is written
     * @param link_name name of the link for which to compute the angular acceleration
     */
    void angularAcceleration(Eigen::Vector3d& aaccel,
                             const std::string& link_name);

    /**
     * @brief returns the number of degrees of freedom of the robot
     */
    int dof();

    /**
     * @brief Gives orientation error from rotation matrices
     * @param oerr Vector on which the orientation error will be written
     * @param desired_orientation desired orientation rotation matrix
     * @param current_orientation current orientation matrix
     */
    void orientationError(Eigen::Vector3d& delta_phi,
    		              const Eigen::Matrix3d& desired_orientation,
    		              const Eigen::Matrix3d& current_orientation);


    /**
     * @brief Gives orientation error from quaternions
     * @param oerr Vector on which the orientation error will be written
     * @param desired_orientation desired orientation quaternion
     * @param current_orientation current orientation quaternion
     */
    void orientationError(Eigen::Vector3d& delta_phi,
    		              const Eigen::Quaterniond& desired_orientation,
    		              const Eigen::Quaterniond& current_orientation);

    void taskInertiaMatrix(Eigen::MatrixXd& Lambda,
    					   const Eigen::MatrixXd& task_jacobian);

    void dynConsistentInverseJacobian(Eigen::MatrixXd& Jbar,
    								  const Eigen::MatrixXd& task_jacobian);


    void nullspaceMatrix(Eigen::MatrixXd& N,
        					 const Eigen::MatrixXd& jacobian);

    void nullspaceMatrix(Eigen::MatrixXd& N,
        					 const Eigen::MatrixXd& jacobian,
        					 const Eigen::MatrixXd& N_prec);

    ModelInternal* _model_internal;

    ////// Robot State ///////

    /// \brief Joint positions
    Eigen::VectorXd _q;

    /// \brief Joint velocities
    Eigen::VectorXd _dq;

    /// \brief Joint accelerations
    Eigen::VectorXd _ddq;

    /// \brief Mass Matrix
    Eigen::MatrixXd _M;

    /// \brief Inverse of the mass matrix
    Eigen::MatrixXd _M_inv;

};



}
#endif /* MODELINTERFACE_H_ */
