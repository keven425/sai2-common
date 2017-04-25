/**
 * \file ChaiGraphics.h
 *
 *  Created on: Dec 30, 2016
 *      Author: Shameek Ganguly
 */

#ifndef CHAI_GRAPHICS_H
#define CHAI_GRAPHICS_H

#include "GraphicsInternal.h"
#include <chai3d.h>
#include "chai_extension/CRobotLink.h"

namespace Graphics {

class ChaiGraphics: public GraphicsInternal {
public:
	/**
     * @brief Creates a Chai graphics interface object that contains a visual model of the virtual world.
     * @param path_to_world_file A path to the file containing the model of the virtual world (urdf and yml files supported).
     * @param parser The type of parser to use in function of the file type (urdf or yml).
     * @param verbose To display information about the robot model creation in the terminal or not.
     */
	ChaiGraphics(const std::string& path_to_world_file,
					Graphics::ParserType parser,
					bool verbose = false);

	// dtor
	~ChaiGraphics();

	/**
     * @brief Update the graphics model for a robot in the virtual world.
     * @param robot_name Name of the robot for which model update is considered.
     * @param robot_model A ModelInterface object for this robot to be used to get intermediate transforms.
     */
	void updateGraphics(const std::string& robot_name,
						Model::ModelInterface* robot_model);

	/**
     * @brief Render the virtual world to the current context.
     * 	NOTE: the correct context should have been selected prior to this.
     * @param camera_name Camera name to be rendered from.
     * @param window_width Width of viewport in screen co-ordinates.
     * @param window_height Height of viewport in screen co-ordinates.
     * @param display_context_id ID for the context to display in. This ID is only to be used
     *	for selective rendering. It does not change the GL context to render to.
     */
	void render(const std::string& camera_name,
				int window_width, 
				int window_height, 
				int display_context_id = 0);

     /**
     * @brief Return the pose of the camera in the parent frame
     * @param camera_name Camera name.
     * @param ret_position Position of the camera.
     * @param ret_vertical Up vector for the camera.
     * @param ret_lookat Point the camera is looking at.
     */
     void getCameraPose(const std::string& camera_name,
                         Eigen::Vector3d& ret_position,
                         Eigen::Vector3d& ret_vertical,
                         Eigen::Vector3d& ret_lookat);

     /**
     * @brief Sets the pose of the camera in the parent frame
     * @param camera_name Camera name.
     * @param position Position of the camera.
     * @param vertical Up vector for the camera.
     * @param lookat Point the camera is to look at.
     */
     void setCameraPose(const std::string& camera_name,
                         const Eigen::Vector3d& position,
                         const Eigen::Vector3d& vertical,
                         const Eigen::Vector3d& lookat);

     /**
     * @brief Get info about link of the specified robot at the given cursor position.
     * @return True if a link is present, False if not
     * @param camera_name Camera name.
     * @param robot_name Name of robot to look for.
     * @param view_x x-position of cursor in viewport (OpenGL style screen co-ordinates).
     * @param view_y y-position of cursor in viewport (OpenGL style screen co-ordinates).
     * @param window_width Width of viewport in screen co-ordinates.
     * @param window_height Height of viewport in screen co-ordinates.
     * @param ret_link_name Name of the link. Garbage if no link present at cursor location.
     * @param ret_pos Position of cursor in link frame. Garbage if no link present at cursor location.
     */
     bool getRobotLinkInCamera(const std::string& camera_name,
                                   const std::string& robot_name,
                                   int view_x,
                                   int view_y,
                                   int window_width,
                                   int window_height,
                                   std::string& ret_link_name,
                                   Eigen::Vector3d& ret_pos);

     /* CHAI specific interface */
     /**
     * @brief Get pointer to Chai camera object.
     * @param camera_name Camera name.
     */
     chai3d::cCamera* getCamera(const std::string& camera_name);

         /**
     * @brief recursively search for cRobotLink from cGenericObject parent
     * @return cRobotLink with link_name (NULL if none found)
     * @param link_name Link name
     * @param parent cGenericObject parent you are searching the link in
     */
     chai3d::cRobotLink* searchLinkInParent(const std::string& link_name, chai3d::cGenericObject* parent);

          /**
     * @brief get robot link from robot
     * @return cRobotLink with link_name (will abort if no link found)
     * @param link_name Link name
     * @param robot_name the name of the robot (will search in world for robot)
     */
     chai3d::cRobotLink* getLink(const std::string& link_name, const std::string& robot_name);
     

public:
	/**
     * @brief Internal cWorld object.
     */
	chai3d::cWorld* _world;

};

}

#endif //CHAI_GRAPHICS_H
