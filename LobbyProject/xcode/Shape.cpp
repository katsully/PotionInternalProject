//
//  Shape.cpp
//  LobbyProject
//
//  Created by Kat Sullivan on 7/2/15.
//
//

#include "Shape.h"

Shape::Shape() :
centroid( cv::Point() ),
ID(-1),
lastFrameSeen(-1),
matchFound(false),
moving(false),
stillness(0)
{
}