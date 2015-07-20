//
//  Shape.h
//  LobbyProject
//
//  Created by Kat Sullivan on 7/1/15.
//
//

#pragma once
#include "CinderOpenCV.h"

class Shape {
public:
    Shape();
    
    int ID;
    double area;
    float depth;
    cv::Point centroid; // center point of the shape
    Boolean matchFound;
    bool moving;
    cv::vector<cv::Point> hull; // stores point representing the hull of the shape
    int lastFrameSeen;  // mark the last frame where the blob was seen, used to track when shapes leave the frame
};
