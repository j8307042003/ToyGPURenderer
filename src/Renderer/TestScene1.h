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
	scene.AddMaterial("Right Wall", new material({0.9, 0.4, 0.4}, {0, 0, 0}, 0.0f));	 //Right Wall
	scene.AddMaterial("Left Wall ", new material({0.4, 0.9, 0.4}, {0, 0, 0}, 0.0f));	 //Left Wall
	// scene.AddMaterial("Behind Wall", new material({1, 1, 1}, {1, 1, 1}, 1)); //behind wall
	scene.AddMaterial("Behind Wall", new material({1, 1, 1}, {0, 0, 0}, 0)); //behind wall
	scene.AddMaterial("End Wall", new material({1, 1, 1}, {0, 0, 0}, 0)); //End Wall	
	scene.AddMaterial("Floor Wall", new material({ 1, 1, 1 }, { 0, 0, 0 }, 0.0f, 0));	 //Floor
	scene.AddMaterial("Ceil Wall", new material({1, 1, 1}, {1, 1, 1}, 0.4));	 //Ceil
	// scene.AddMaterial("Ceil Wall", new material({1, 1, 1}, {0, 0, 0}, 0.1f));	 //Ceil
	scene.AddMaterial("Ceil Light", new material({ 1, 1, 1 }, { 1, 1, 1 }, 0.4));	 //Ceil
	// scene.AddMaterial("Ceil Light", new material({0.8, 0.8, 0.8}, {0, 0, 0}, 0.4));	 //Ceil

	// scene.AddMaterial("transparent wall", new material({1, 1, 1}, {0, 0, 0}, .95, .95, .99, 1.33)); //End Wall	
	scene.AddMaterial("transparent wall", new material({1, 1, 1}, {0, 0, 0}, .99, 0, 0, 1.33)); //End Wall	

	//spheres
	/*
	// scene.AddMaterial("s1", new material({1, 1, 1}, {1, 0.5, 0.5}, 0.7));	
	scene.AddMaterial("s1", new material({1, .3, .3}, {0, 0, 0}, 1, 1));	
	// scene.AddMaterial("s2", new material({1, 1, 1}, {0.5, 1, 0.5}, 0.3));	
	scene.AddMaterial("s2", new material({1, .3, .3}, {0, 0, 0}, 0));	

	// scene.AddMaterial("s3", new material({1, 1, 1}, {0, 0, 0}, 0.9, .9));	
	scene.AddMaterial("s3", new material({1, 1, 1}, {0, 0, 0}, 0, .9));	
	// scene.AddMaterial("s3", new material({1, 1, 1}, {0, 0, 0}, .99, .8, .96f, 1.33f));	
	scene.AddMaterial("s4", new material({1, 1, 1}, {0, 0, 0}, 0));	
	// scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 1));	
	scene.AddMaterial("s5", new material({1, 1, 1}, {1, 1, 1}, 0.3));
	*/

	//copper
	scene.AddMaterial("s1", new material({0.955, 0.637, 0.538}, {0, 0, 0}, .6, 1));	

	//sliver
	scene.AddMaterial("s2", new material({0.972, 0.960, 0.915}, {0, 0, 0}, .7, 1));	
	scene.AddMaterial("s3", new material({.1, .1, .1}, {0, 0, 0}, 0.9, .1));	
	scene.AddMaterial("s4", new material({1.000, 0.766, 0.336}, {0, 0, 0}, .2));	
	scene.AddMaterial("s5", new material({1, 1, 1}, {1, 1, 1}, 0.3, 0.8));


	scene.AddMaterial("dragon", new material({1, 1, 1}, {0, 0, 0}, .95, 0, 1));




	const int emptyObj = 30;
	int item = 5;
	int totalRow = emptyObj / item;
	for(int i = 0 ; i < emptyObj; i++) {
		int row = i / item;
		int rowIdx = i % item;
		std::string sphereMatName = "s auto gen " + std::to_string(i); 
		// scene.AddMaterial(sphereMatName, new material({((float) row) / item, ((float) rowIdx) / item, 0}, {0, 0, 0}, .9999, 0, 1, 1.33));	
		// scene.AddMaterial(sphereMatName, new material({1, 1, 1}, {0, 0, 0}, .95, 0, .5, 1.5));	
		scene.AddMaterial(sphereMatName, new material({1, 1, 1}, {0, 0, 0}, .9999, 0, 1, 1.33));	
		// scene.AddMaterial(sphereMatName, new material({((float) row) / item, ((float) rowIdx) / item, 0}, {0, 0, 0}, .1, .1, 0, 1.33));	
		// scene.AddMaterial(sphereMatName, new material({((float) row) / item, ((float) rowIdx) / item, 0}, {0, 0, 0}, ((float) row) / item, 1, 0, 1.33));	
		//scene.AddMaterial(sphereMatName, new material({((float) row) / item, ((float) rowIdx) / item, 0}, {0, 0, 0}, 1.0f, 0.0, 0, 2.5));	
		scene.AddShape(new Sphere({ (((i % item) - 0.5f * item) / item) * wallOffsetX * 2 + 0.6f, (row - totalRow / 2) * (wallHeight / totalRow) * 2 + 0.6f, -wallWidth / 2.0f}, 0.5), 
						sphereMatName);	
		// scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 0.95f));	
	}



	// scene.AddModel("teapot.obj", "s1", 0.03);
	// scene.AddModel("fireplace_room.obj", "s1", 10);
	//scene.AddModel("dragon.obj", "s1", 1);
	scene.AddModel("Mesh001.obj", "dragon", .1);
	// scene.AddModel("testObj.obj", "dragon", 1);

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
	//scene.AddShape(new Plane(
	//	{-wallOffsetX,  -wallHeight, 0.3/*wallWidth / 4*/}, 
	//	{-wallOffsetX,   wallHeight, 0.3/*wallWidth / 4*/}, 
	//	{ wallOffsetX,  -wallHeight, 0.3/*wallWidth / 4*/}, 
	//	{ wallOffsetX,   wallHeight, 0.3/*wallWidth / 4*/}
	//),"Behind Wall");	

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
		
	//Ceil
	scene.AddShape(new Plane(
		{-wallOffsetX / 3.0f,  wallHeight - 0.2f,  wallWidth / 2.0f}, 
		{-wallOffsetX / 3.0f,  wallHeight - 0.2f, -wallWidth / 2.0f}, 
		{ wallOffsetX / 3.0f,  wallHeight - 0.2f,  wallWidth / 2.0f},
		{ wallOffsetX / 3.0f,  wallHeight - 0.2f, -wallWidth / 2.0f}
	),"Ceil Light");

	//End Wall
	scene.AddShape(new Plane(
		{-wallOffsetX / 5.0f,   wallHeight, -wallWidth / 3.0f}, 
		{-wallOffsetX / 5.0f,  -wallHeight, -wallWidth / 3.0f}, 
		{ wallOffsetX / 5.0f,   wallHeight, -wallWidth / 3.0f},	
		{ wallOffsetX / 5.0f,  -wallHeight, -wallWidth / 3.0f}
	),"transparent wall");


	float transparentWallWidth = 1.0;
	//End Wall2
	scene.AddShape(new Plane(
		{-wallOffsetX / 5.0f,   wallHeight, -wallWidth / 3.0f + transparentWallWidth}, 
		{-wallOffsetX / 5.0f,  -wallHeight, -wallWidth / 3.0f + transparentWallWidth}, 
		{ wallOffsetX / 5.0f,   wallHeight, -wallWidth / 3.0f + transparentWallWidth},	
		{ wallOffsetX / 5.0f,  -wallHeight, -wallWidth / 3.0f + transparentWallWidth}
	),"transparent wall");

	//End Wall2
	scene.AddShape(new Plane(
		{-wallOffsetX / 5.0f,   wallHeight, -wallWidth / 3.0f}, 
		{-wallOffsetX / 5.0f,  -wallHeight, -wallWidth / 3.0f}, 
		{-wallOffsetX / 5.0f,   wallHeight, -wallWidth / 3.0f + transparentWallWidth},	
		{-wallOffsetX / 5.0f,  -wallHeight, -wallWidth / 3.0f + transparentWallWidth}
	),"transparent wall");	

	//End Wall2
	scene.AddShape(new Plane(
		{wallOffsetX / 5.0f,   wallHeight, -wallWidth / 3.0f}, 
		{wallOffsetX / 5.0f,  -wallHeight, -wallWidth / 3.0f}, 
		{wallOffsetX / 5.0f,   wallHeight, -wallWidth / 3.0f + transparentWallWidth},	
		{wallOffsetX / 5.0f,  -wallHeight, -wallWidth / 3.0f + transparentWallWidth}
	),"transparent wall");	

	//scene.AddShape(new Sphere({-2.5, -wallHeight + 2, -wallWidth / 1.2f}, 2), "s1");	
	//scene.AddShape(new Sphere({ 2.5, -wallHeight + 2, -wallWidth / 1.2f}, 2), "s2");	
	//scene.AddShape(new Sphere({  0, -wallHeight + 2, -wallWidth / 1.2f + 2.5f}, 2), "s3");	
	//scene.AddShape(new Sphere({  0, -wallHeight + 2, -wallWidth / 1.2f - 2.5f}, 2), "s4");	
	//scene.AddShape(new Sphere({  0, wallHeight - 1, -wallWidth / 1.2f}, 2), "s5");	

	scene.AddShape(new Sphere({-3.5, -wallHeight + 1, -wallWidth / 1.2f}, 1), "s1");	
	scene.AddShape(new Sphere({  -1, -wallHeight + 1, -wallWidth / 1.2f}, 1), "s2");	
	scene.AddShape(new Sphere({ 1.5,  -wallHeight + 1, -wallWidth / 1.2f}, 1), "s3");	
	scene.AddShape(new Sphere({ 3.5,  -wallHeight + 1, -wallWidth / 1.2f}, 1), "s4");	
	// scene.AddShape(new Sphere({   0,   wallHeight - 2.0f, -wallWidth / 1.2f}, 2), "s5");	


	// scene.AddShape(new Sphere({ 0, 0, -wallWidth / 2.f}, 0.7));	
	// scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}, 0.7));	


	// scene.AddShape(new Sphere({ -3, 0, -wallWidth / 2.5f}, 1));	
	// scene.AddMaterial(new material({1, 1, 1}, {4, 4, 4}, 0.3));	

	// scene.AddShape(new Sphere({ 3, 0, -wallWidth / 2.5f}, 1));	
	// scene.AddMaterial(new material({1, 1, 1}, {4, 4, 4}, 0.3));	

	return scene;
}
