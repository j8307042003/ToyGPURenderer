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
	// scene.AddShape(new Sphere({ 0, 3, -wallWidth / 1.0f}, 2));	

	scene.AddShape(new Sphere({ 0, 5, -wallWidth / 1.0f}, 2));	
	scene.AddShape(new Sphere({ 3, 3, -wallWidth / 1.0f}, 2));	
	scene.AddShape(new Sphere({ -4, 3, -wallWidth / 1.0f}, 2));	
	scene.AddShape(new Sphere({ 0,  0, -wallWidth / 1.0f}, 2));	


	//walls
	scene.AddMaterial(new material({0.7, 0.3, 0.3}, {0, 0, 0}));	 //Right Wall
	scene.AddMaterial(new material({0.3, 0.7, 0.3}, {0, 0, 0}));	 //Left Wall
	scene.AddMaterial(new material({0.7, 0.7, 0.7}, {0, 0, 0})); //behind wall
	scene.AddMaterial(new material({0.7, 0.7, 0.7}, {0, 0, 0})); //End Wall	
	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}));	 //Floor
	scene.AddMaterial(new material({1, 1, 1}, {1, 1, 1}));	 //Ceil

	//spheres
	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}));	
	// scene.AddMaterial(new material({1, 1, 1}, {0.4, 0.4, 0.4}));	

	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}));	
	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}));	
	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}));	
	scene.AddMaterial(new material({1, 1, 1}, {0, 0, 0}));	



	return scene;
}