/*
  Copyright ©2013 The Regents of the University of California
  (Regents). All Rights Reserved. Permission to use, copy, modify, and
  distribute this software and its documentation for educational,
  research, and not-for-profit purposes, without fee and without a
  signed licensing agreement, is hereby granted, provided that the
  above copyright notice, this paragraph and the following two
  paragraphs appear in all copies, modifications, and
  distributions. Contact The Office of Technology Licensing, UC
  Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley, CA 94720-1620,
  (510) 643-7201, for commercial licensing opportunities.

  IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT,
  INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
  LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
  DOCUMENTATION, EVEN IF REGENTS HAS BEEN ADVISED OF THE POSSIBILITY
  OF SUCH DAMAGE.

  REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING
  DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS PROVIDED "AS
  IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
  UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/


#include "display.hpp"
#include "displayphysics.hpp"
#include "displayreplay.hpp"
#include "displaytesting.hpp"
#include "misc.hpp"
#include "io.hpp"
#include "runphysics.hpp"

#include "dart/dart.hpp"
#include "dart/gui/gui.hpp"
#if HAVE_BULLET_COLLISION
  #include "dart/collision/bullet/bullet.hpp"
#endif
#include <cstdlib>
// /home/krati/dinarcsim/dart
using namespace std;
using namespace dart::common;
using namespace dart::dynamics;
using namespace dart::simulation;
using namespace dart::gui;
using namespace dart::utils;
using namespace dart::math;

const double default_speed_increment = 0.5;

const int default_ik_iterations = 4500;

const double default_height = 1.0; // m
const double default_width = 0.2;  // m
const double default_depth = 0.2;  // m

const double default_torque = 15.0; // N-m
const double default_force =  15.0; // N
const int default_countdown = 200;  // Number of timesteps for applying force



class MyWindow : public dart::gui::SimWindow
{
public:

  /// Constructor
  MyWindow(WorldPtr world)

  {
    setWorld(world);

    // Find the Skeleton named "cube" within the World
    mcube = world->getSkeleton("cube");


  }

  void timeStepping() override
  {
    // Reset all the shapes to be Blue
    // Lesson 1a

    // Step the simulation forward
    SimWindow::timeStepping();
  }

protected:


  /// The cube that we will be perturbing
  SkeletonPtr mcube;

}; 


// implement this anywhere in any .cpp file
extern void debug (const vector<string> &args);

int main (int argc, char **argv) {
    dart::simulation::WorldPtr myWorld
      = dart::utils::SkelParser::readWorld("/home/krati/dinarcsim/data/skel/MyManipulator2.skel");
    assert(myWorld != nullptr);
    //cout<<"I reached here"<<endl;
  Eigen::Vector3d gravity(0.0, -9.81, 0.0);
  myWorld->setGravity(gravity);
  
    struct Action {
        string name;
        void (*run) (const vector<string> &args);
    } actions[] = {
        {"simulate", display_physics},
        {"simulateoffline", run_physics},
        {"resume", display_resume},
        {"resumeoffline", resume_physics},
        {"replay", display_replay},
        {"merge", merge_meshes},
        {"split", split_meshes},
        {"test", display_testing},
        {"tri2obj", tri2obj},
        {"debug", debug}
    };
    int nactions = sizeof(actions)/sizeof(Action);
    string name = (argc <= 1) ? "" : argv[1];
    vector<string> args;
    for (int a = 2; a < argc; a++)
        args.push_back(argv[a]);
    for (int i = 0; i < nactions; i++) {
        if (name == actions[i].name) {
            actions[i].run(args);
            exit(EXIT_SUCCESS);
        }
    }
    cout << "Usage: " << endl;
    cout << "    " << argv[0] << " <command> [<args>]" << endl;
    cout << "where <command> is one of" << endl;
    for (int i = 0; i < nactions; i++)
        cout << "    " << actions[i].name << endl;
    cout << endl;
    cout << "Run '" << argv[0] << " <command>' without extra arguments ";
    cout << "to get more information about a particular command." << endl;
    


    exit(EXIT_FAILURE);
}
