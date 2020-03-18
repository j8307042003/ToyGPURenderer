#include "Scene.h"
#include "shape/Sphere.h"
#include "shape/Triangle.h"
#include<iostream>
#include "math/Transform.h"
#include "shape/Plane.h"
#include "Material.h"

Scene make_test_scene1() {
	auto scene = Scene();

	// scene.AddShape(new Sphere({0, 0, 10}, 2));	
	// scene.AddShape(new Sphere({0, 0, -10}, 2));	
	// scene.AddShape(new Sphere({1, 2, 16}, 2));	
	// scene.AddShape(new Sphere({2, 1, 7}, 2));	
	// scene.AddShape(new Triangle({0, 1, 3}, { 1, 0, 3}, {0, 0, 3}));	
	// scene.AddShape(new Triangle({0, 0, 3}, {0, 1, 3}, { 1, 0, 3}));	
	// scene.AddShape(new Triangle({0, 0, 6}, {0, 1, 6}, { 1, 0, 6}));	


	// CORNELL Box
	float wallOffsetX = 5;
	float wallHeight = 4;
	float wallWidth  = 20;

	//Right Wall
	scene.AddShape(new Plane(
		{wallOffsetX,  wallHeight, wallWidth},	
		{wallOffsetX,  wallHeight, -wallWidth},
		{wallOffsetX, -wallHeight, wallWidth}, 
		{wallOffsetX, -wallHeight, -wallWidth}
	));

	//Left Wall
	scene.AddShape(new Plane(
		{-wallOffsetX, -wallHeight, -wallWidth}, 
		{-wallOffsetX,  wallHeight, -wallWidth}, 
		{-wallOffsetX, -wallHeight, wallWidth}, 
		{-wallOffsetX,  wallHeight, wallWidth}
		));


	//behind Cam wall
	scene.AddShape(new Plane(
		{-wallOffsetX,  -wallHeight, wallWidth}, 
		{-wallOffsetX,   wallHeight, wallWidth}, 
		{ wallOffsetX,  -wallHeight, wallWidth}, 
		{ wallOffsetX,   wallHeight, wallWidth}
	));	

	//End Wall
	scene.AddShape(new Plane(
		{-wallOffsetX,   wallHeight, -wallWidth}, 
		{-wallOffsetX,  -wallHeight, -wallWidth}, 
		{ wallOffsetX,   wallHeight, -wallWidth},	
		{ wallOffsetX,  -wallHeight, -wallWidth}
	)); 

	//Floor
	scene.AddShape(new Plane(
		{-wallOffsetX,  -wallHeight, -wallWidth}, 
		{-wallOffsetX,  -wallHeight,  wallWidth}, 
		{ wallOffsetX,  -wallHeight, -wallWidth}, 
		{ wallOffsetX,  -wallHeight,  wallWidth}
	));		

	//Ceil
	scene.AddShape(new Plane(
		{-wallOffsetX,  wallHeight,  wallWidth}, 
		{-wallOffsetX,  wallHeight, -wallWidth}, 
		{ wallOffsetX,  wallHeight,  wallWidth},
		{ wallOffsetX,  wallHeight, -wallWidth}
	));	

	// scene.AddShape(new Sphere({0, 0, wallWidth / 2}, 1));	
	scene.AddShape(new Sphere({-2.5, 0, -wallWidth / 1.2f}, 2));	
	scene.AddShape(new Sphere({ 1, 0, -wallWidth / 1.2f}, 2));	

	scene.AddShape(new Sphere({ 0, 5, -wallWidth / 1.5f}, 2));	
	scene.AddShape(new Sphere({ 3, 3, -wallWidth / 1.5f}, 2));	
	scene.AddShape(new Sphere({ -4, 3, -wallWidth / 1.5f}, 2));	
	scene.AddShape(new Sphere({ 0,  0, -wallWidth / 1.5f}, 2));	


	//walls
	scene.AddMaterial(new material({0.7, 0.3, 0.3}, {0, 0, 0}, 0));	 //Right Wall
	scene.AddMaterial(new material({0.3, 0.7, 0.3}, {0, 0, 0}, 0.4));	 //Left Wall
	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 0)); //behind wall
	scene.AddMaterial(new material({1, 1, 1}, {0.2, 0.5, 0.2}, 0)); //End Wall	
	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 0));	 //Floor
	scene.AddMaterial(new material({0.8, 0.8, 0.8}, {1, 1, 1}, 0));	 //Ceil

	//spheres
	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 0.7));	
	scene.AddMaterial(new material({1, 1, 1}, {0.4, 0.4, 1}, 0.3));	

	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 0.3));	
	scene.AddMaterial(new material({1, 0.4, 0.4}, {0, 0, 0}, 1));	
	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 0.3));	
	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 0.8));	



	return scene;
}



Scene make_test_scene2() {
	auto scene = Scene();

	// scene.AddShape(new Sphere({0, 0, 10}, 2));	
	// scene.AddShape(new Sphere({0, 0, -10}, 2));	
	// scene.AddShape(new Sphere({1, 2, 16}, 2));	
	// scene.AddShape(new Sphere({2, 1, 7}, 2));	
	// scene.AddShape(new Triangle({0, 1, 3}, { 1, 0, 3}, {0, 0, 3}));	
	// scene.AddShape(new Triangle({0, 0, 3}, {0, 1, 3}, { 1, 0, 3}));	
	// scene.AddShape(new Triangle({0, 0, 6}, {0, 1, 6}, { 1, 0, 6}));	


	// CORNELL Box
	float wallOffsetX = 5;
	float wallHeight = 4;
	float wallWidth  = 40;


	//walls
	scene.AddMaterial("Right Wall", new material({0.7, 0.3, 0.3}, {0, 0, 0}, 0.3));	 //Right Wall
	scene.AddMaterial("Left Wall ", new material({0.3, 0.7, 0.3}, {0, 0, 0}, 0.3));	 //Left Wall
	scene.AddMaterial("Behind Wall", new material({1, 1, 1}, {6, 6, 6}, 1)); //behind wall
	scene.AddMaterial("End Wall", new material({1, 1, 1}, {0, 0, 0}, 0.3)); //End Wall	
	scene.AddMaterial("Floor Wall", new material({1, 1, 1}, {0, 0, 0}, 0.3));	 //Floor
	scene.AddMaterial("Ceil Wall", new material({0.8, 0.8, 0.8}, {0.9, 0.9, 0.9}, 0.4));	 //Ceil

	//spheres
	scene.AddMaterial("s1", new material({1, 1, 1}, {3, 0.5, 0.5}, 0.7));	
	scene.AddMaterial("s2", new material({1, 1, 1}, {0.5, 3, 0.5}, 0.3));	

	// scene.AddMaterial("s3", new material({1, 1, 1}, {0, 0, 0}, 0));	
	scene.AddMaterial("s3", new material({1, 1, .1}, {0, 0, 0}, 0, .1));	
	scene.AddMaterial("s4", new material({1, 1, 1}, {0, 0, 0}, 0.5));	
	// scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 1));	
	scene.AddMaterial("s5", new material({1, 1, 1}, {1, 1, 1}, 0.3));	

	const int emptyObj = 30;
	int item = 5;
	int totalRow = emptyObj / item;
	for(int i = 0 ; i < emptyObj; i++) {
		int row = i / item;
		int rowIdx = i % item;
		std::string sphereMatName = "s auto gen " + std::to_string(i); 
		scene.AddMaterial(sphereMatName, new material({((float) row) / item, ((float) rowIdx) / item, 0}, {0, 0, 0}, i / (float)item, .7));	
		scene.AddShape(new Sphere({ (((i % item) - 0.5f * item) / item) * wallOffsetX * 2 + 0.5f, (row - totalRow / 2) * (wallHeight / totalRow) * 2, -wallWidth / 2.0f}, 0.5), 
						sphereMatName);	
		// scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 0.95f));	
	}






	//Right Wall
	scene.AddShape(new Plane(
		{wallOffsetX,  wallHeight, wallWidth},	
		{wallOffsetX,  wallHeight, -wallWidth},
		{wallOffsetX, -wallHeight, wallWidth}, 
		{wallOffsetX, -wallHeight, -wallWidth}
	),"Right Wall");

	//Left Wall
	scene.AddShape(new Plane(
		{-wallOffsetX, -wallHeight, -wallWidth}, 
		{-wallOffsetX,  wallHeight, -wallWidth}, 
		{-wallOffsetX, -wallHeight, wallWidth}, 
		{-wallOffsetX,  wallHeight, wallWidth}
		),"Left Wall ");


	//behind Cam wall
	scene.AddShape(new Plane(
		{-wallOffsetX,  -wallHeight, 0.3/*wallWidth / 4*/}, 
		{-wallOffsetX,   wallHeight, 0.3/*wallWidth / 4*/}, 
		{ wallOffsetX,  -wallHeight, 0.3/*wallWidth / 4*/}, 
		{ wallOffsetX,   wallHeight, 0.3/*wallWidth / 4*/}
	),"Behind Wall");	

	//End Wall
	scene.AddShape(new Plane(
		{-wallOffsetX,   wallHeight, -wallWidth}, 
		{-wallOffsetX,  -wallHeight, -wallWidth}, 
		{ wallOffsetX,   wallHeight, -wallWidth},	
		{ wallOffsetX,  -wallHeight, -wallWidth}
	),"End Wall"); 

	//Floor
	scene.AddShape(new Plane(
		{-wallOffsetX,  -wallHeight, -wallWidth}, 
		{-wallOffsetX,  -wallHeight,  wallWidth}, 
		{ wallOffsetX,  -wallHeight, -wallWidth}, 
		{ wallOffsetX,  -wallHeight,  wallWidth}
	),"Floor Wall");		

	//Ceil
	scene.AddShape(new Plane(
		{-wallOffsetX,  wallHeight,  wallWidth}, 
		{-wallOffsetX,  wallHeight, -wallWidth}, 
		{ wallOffsetX,  wallHeight,  wallWidth},
		{ wallOffsetX,  wallHeight, -wallWidth}
	),"Ceil Wall");	
		

	// scene.AddShape(new Sphere({0, 0, wallWidth / 2}, 1));	
	scene.AddShape(new Sphere({-2.5, -wallHeight + 2, -wallWidth / 1.2f}, 2), "s1");	
	scene.AddShape(new Sphere({ 2.5, -wallHeight + 2, -wallWidth / 1.2f}, 2), "s2");	

	scene.AddShape(new Sphere({  0, -wallHeight + 2, -wallWidth / 1.2f + 2.5f}, 2), "s3");	
	scene.AddShape(new Sphere({  0, -wallHeight + 2, -wallWidth / 1.2f - 2.5f}, 2), "s4");	
	// scene.AddShape(new Sphere({  0, -wallHeight + 2 + 2, -wallWidth / 1.2f}, 2));	
	scene.AddShape(new Sphere({  0, wallHeight - 1, -wallWidth / 1.2f}, 2), "s5");	




	// scene.AddShape(new Sphere({ 0, 0, -wallWidth / 2.f}, 0.7));	
	// scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 0.7));	


	// scene.AddShape(new Sphere({ -3, 0, -wallWidth / 2.5f}, 1));	
	// scene.AddMaterial(new material({1, 1, 1}, {4, 4, 4}, 0.3));	

	// scene.AddShape(new Sphere({ 3, 0, -wallWidth / 2.5f}, 1));	
	// scene.AddMaterial(new material({1, 1, 1}, {4, 4, 4}, 0.3));	

	return scene;
}
