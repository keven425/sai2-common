// This example application loads a URDF world file and simulates two robots
// with physics and contact in a Dynamics3D virtual world. A graphics model of it is also shown using 
// Chai3D.

#include "model/ModelInterface.h"
#include "simulation/SimulationInterface.h"
#include "simulation/Sai2Simulation.h"
#include "graphics/GraphicsInterface.h"
#include "force_sensor/ForceSensorSim.h"
#include "force_sensor/ForceSensorDisplay.h"

#include <GLFW/glfw3.h> //must be loaded after loading opengl/glew

#include <iostream>
#include <string>

using namespace std;

const string world_file = "resources/world.urdf";
const string robot_file = "resources/pbot.urdf";
const string robot_name1 = "PBot1";
// const string robot_name2 = "PBot2";
const string camera_name = "camera_fixed";

unsigned long long simulation_counter = 0;

// callback to print glfw errors
void glfwError(int error, const char* description);

// callback when a key is pressed
void keySelect(GLFWwindow* window, int key, int scancode, int action, int mods);

int main() {
	cout << "Loading URDF world model file: " << world_file << endl;

	// load simulation world
	auto sim = new Simulation::Sai2Simulation(world_file, Simulation::urdf, false);

	// load graphics scene
	// auto graphics = new Graphics::GraphicsInterface(world_file, Graphics::chai, Graphics::urdf, true);
	auto graphics = new Graphics::ChaiGraphics(world_file, Graphics::urdf, true);

	// load robots
	auto robot1 = new Model::ModelInterface(robot_file, Model::rbdl, Model::urdf, false);

	// create force sensor
	ForceSensorSim* force_sensor = new ForceSensorSim(robot_name1, "link0", Eigen::Affine3d::Identity(), robot1);
	ForceSensorDisplay* force_display = new ForceSensorDisplay(force_sensor, graphics);
	Eigen::Vector3d force, moment;

	// offset a joint initial condition
	sim->getJointPositions(robot_name1, robot1->_q);
	sim->setJointPosition(robot_name1, 0, robot1->_q[0] + 0.5);
	
	/*------- Set up visualization -------*/
    // set up error callback
    glfwSetErrorCallback(glfwError);

    // initialize GLFW
    glfwInit();

    // retrieve resolution of computer display and position window accordingly
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    // information about computer screen and GLUT display window
	int screenW = mode->width;
    int screenH = mode->height;
    int windowW = 0.8 * screenH;
    int windowH = 0.5 * screenH;
    int windowPosY = (screenH - windowH) / 2;
    int windowPosX = windowPosY;

    // create window and make it current
    glfwWindowHint(GLFW_VISIBLE, 0);
    GLFWwindow* window = glfwCreateWindow(windowW, windowH, "05-force_sensor", NULL, NULL);
	glfwSetWindowPos(window, windowPosX, windowPosY);
	glfwShowWindow(window);
    glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

    // set callbacks
	glfwSetKeyCallback(window, keySelect);

	sim->setCollisionRestitution(0);

    // while window is open:
    while (!glfwWindowShouldClose(window))
	{
		// update simulation by 1ms
		sim->integrate(0.01);

		// update kinematic models
		sim->getJointPositions(robot_name1, robot1->_q);
		sim->getJointVelocities(robot_name1, robot1->_dq);
		robot1->updateModel();

		// update force sensor and display
		force_sensor->update(sim);
		force_sensor->getForce(force);
		force_sensor->getMoment(moment);
		force_display->update();

		// update graphics. this automatically waits for the correct amount of time
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		graphics->updateGraphics(robot_name1, robot1);
		// graphics->updateGraphics(robot_name2, robot2);
		graphics->render(camera_name, width, height);

		// swap buffers
		glfwSwapBuffers(window);

		// wait until all GL commands are completed
		glFinish();

		// check for any OpenGL errors
		GLenum err;
		err = glGetError();
		assert(err == GL_NO_ERROR);

	    // poll for events
		glfwPollEvents();

		if(simulation_counter %500 == 0)
		{
			std::cout << "force :\t" << force.transpose() << std::endl;
			std::cout << "moment :\t" << moment.transpose() << std::endl << std::endl;
		}

		simulation_counter++;
	}

    // destroy context
    glfwDestroyWindow(window);

    // terminate
    glfwTerminate();

	return 0;
}

//------------------------------------------------------------------------------

void glfwError(int error, const char* description) {
	cerr << "GLFW Error: " << description << endl;
	exit(1);
}

//------------------------------------------------------------------------------

void keySelect(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // option ESC: exit
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        // exit application
         glfwSetWindowShouldClose(window, 1);
    }
}
