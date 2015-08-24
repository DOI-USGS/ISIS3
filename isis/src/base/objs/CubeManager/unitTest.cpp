#include "CubeManager.h"
#include "FileName.h"
#include "Cube.h"
#include "Preference.h"

#include <iostream>

#include <QVector>

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  std::cout << "CubeManager Unit Test" << std::endl;
  QVector<Cube *> cubes;

  // Read Cubes Into Memory
  cubes.push_back(CubeManager::Open("$base/testData/isisTruth.cub"));
  cubes.push_back(CubeManager::Open("$base/testData/isisTruth.cub"));
  cubes.push_back(CubeManager::Open("$base/testData/blobTruth.cub"));
  cubes.push_back(CubeManager::Open("$base/testData/blobTruth.cub"));
  cubes.push_back(CubeManager::Open("$base/testData/isisTruth.cub+1"));
  cubes.push_back(CubeManager::Open("$base/testData/isisTruth.cub+1"));

  // Print Cube FileNames To Verify We Have Correct Ones
  std::cout << "Verify proper cubes have been read" << std::endl;
  std::cout << "Cube FileNames: " << std::endl;

  for(int i = 0; i < (int)cubes.size(); i++) {
    std::cout << "  " << i + 1 << " : " << FileName(cubes[i]->fileName()).baseName() << std::endl;
  }

  std::cout << std::endl;

  // Print Cube Pointer Comparisons (1 if match, 0 if differ)
  std::cout << "Verify We Don't Have Duplicates..." << std::endl;

  // Print Table Header... up to 9 (for formatting)
  std::cout << "  Cube #";
  for(int i = 0; i < 9 && i < (int)cubes.size(); i++) {
    std::cout << " | Equals " << i + 1;
  }
  std::cout << std::endl;

  // Print Comparison Table Data
  for(int i = 0; i < (int)cubes.size(); i++) {
    std::cout << "  Cube " << i + 1;

    for(int j = 0; j < (int)cubes.size(); j++) {
      std::cout << " |        " << (int)(cubes[i] == cubes[j]);
    }

    std::cout << std::endl;
  }

  std::cout << std::endl;

  // Test cube attributes presence, input attributes only affect bands
  std::cout << "Verify cube attributes have been taken into account" << std::endl;
  std::cout << "  Cube # | # of Bands" << std::endl;
  for(int i = 0; i < (int)cubes.size(); i++) {
    std::cout << "  Cube " << i + 1 << " | " << cubes[i]->bandCount() << std::endl;
  }
  std::cout << std::endl;

  // Test CleanUp() methods
 
  // Try cleaning up a cube that isn't managed by CubeManager
  CubeManager::CleanUp("unmanagedCube.cub");

  // Clean up one of the managed cubes
  CubeManager::CleanUp("$base/testData/blobTruth.cub");

  // Print Cube FileNames to verify that we have cleaned isisTruth cubes correctly
  std::cout << "Verify blobTruth cubes have been cleaned" << std::endl;
  std::cout << "Cube FileNames: " << std::endl;

  for (int i = 0; i < (int)cubes.size(); i++) {
    std::cout << "  " << i + 1 << " : " << FileName(cubes[i]->fileName()).baseName() << std::endl;
  }
  std::cout << std::endl;

  // Clean up remaining cubes
  CubeManager::CleanUp();

  // Print Cube FileNames to verify that we have cleaned remaining cubes correctly
  std::cout << "Verify remaining cubes have been cleaned" << std::endl;
  std::cout << "Cube FileNames: " << std::endl;

  for (int i = 0; i < (int)cubes.size(); i++) {
    std::cout << "  " << i + 1 << " : " << FileName(cubes[i]->fileName()).baseName() << std::endl;
  }
  std::cout << std::endl;

  // Create a CubeManager instance (implicitly test destructor)
  QVector<Cube *> cubes2;
  CubeManager mgr;
  cubes2.push_back(mgr.OpenCube("$base/testData/isisTruth.cub"));
  cubes2.push_back(mgr.OpenCube("$base/testData/blobTruth.cub"));

  // Test setting an opened cube limit
  mgr.SetNumOpenCubes(2);
  std::cout << "Set number of open cubes to 2" << std::endl;
  std::cout << endl;

  // Print Cube FileNames to verify the currently managed cubes
  std::cout << "Currently managed cubes:" << std::endl;
  for (int i = 0; i < (int)cubes2.size(); i++) {
    std::cout << "  " << i + 1 << " : " << FileName(cubes2[i]->fileName()).baseName() << std::endl;
  }
  std::cout << std::endl;

  // This will test the cleanup in OpenCube(const QString &)
  cubes2.push_back(mgr.OpenCube("$base/testData/isisTruth2.cub"));
  // we pop the front because OpenCube should dequeue and clean 1 item to enforce limit
  cubes2.pop_front();
  std::cout << "Opened isisTruth2.cub." << std::endl;
  std::cout << "Verify isisTruth2.cub is now managed and limit of 2 is enforced:" << std::endl;
  for (int i = 0; i < (int)cubes2.size(); i++) {
    std::cout << "  " << i + 1 << " : " << FileName(cubes2[i]->fileName()).baseName() << std::endl;
  }
  std::cout << std::endl;

  // Cleanup
  mgr.CleanCubes();

  // Set a open cube limit that exceeds the system open file limit
  std::cout << "Setting number of open cubes > 60 percent of system open file limit" << std::endl;
  mgr.SetNumOpenCubes(1000000);
  std::cout << std::endl;
  
  // Open a cube that DNE
  std::cout << "Attempting to open a file that does not exist:" << std::endl;
  try {
    mgr.OpenCube("dne.cub");
  } catch (IException &e) {
    std::cout << "  " << e.what() << std::endl;
  }

}
