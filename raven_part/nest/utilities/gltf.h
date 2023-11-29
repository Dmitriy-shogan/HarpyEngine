/*
 * gltf.h
 *
 *  Created on: Aug 22, 2023
 *      Author: hexi
 */

#ifndef RAVEN_PART_NEST_UTILITIES_GLTF_H_
#define RAVEN_PART_NEST_UTILITIES_GLTF_H_

#include <utilities/initializations.h>
namespace harpy::utilities {


#define GLTF_CAMERA_TYPE_PERSPECTIVE "perspective"
#define GLTF_CAMERA_TYPE_ORTHOGRAPHIC "orthographic"
#define GLTF_CAMERA_TYPE_ISOMETRIC "isometric"



//bool loadModel(tinygltf::Model &model, const char *filename) {
//  tinygltf::TinyGLTF loader;
//  std::string err;
//  std::string warn;
//
//  bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
//  if (!warn.empty()) {
//    std::cout << "WARN: " << warn << std::endl;
//  }
//
//  if (!err.empty()) {
//    std::cout << "ERR: " << err << std::endl;
//  }
//
//  if (!res)
//    std::cout << "Failed to load glTF: " << filename << std::endl;
//  else
//    std::cout << "Loaded glTF: " << filename << std::endl;
//
//  return res;
//}

}



#endif /* RAVEN_PART_NEST_UTILITIES_GLTF_H_ */
