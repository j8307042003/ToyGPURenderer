#include "Scene.h"



void Scene::AddShape(Shape * s) {
	shapes.push_back(s);
}


void Scene::AddMaterial(material * s) {
	materials.push_back(s);
}
