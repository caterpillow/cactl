/**
 * Author: Ulf Lundstrom
 * Date: 2009-04-07
 * License: CC0
 * Source: My geometric reasoning
 * Description: Returns the shortest distance on the sphere with radius radius between the points
 * with azimuthal angles (longitude) f1 ($\phi_1$) and f2 ($\phi_2$) from x axis and zenith angles
 * (latitude) t1 ($\theta_1$) and t2 ($\theta_2$) from z axis (0 = north pole). All angles measured
 * in radians. The algorithm starts by converting the spherical coordinates to cartesian coordinates
 * so if that is what you have you can use only the two last rows. dx*radius is then the difference
 * between the two points in the x direction and d*radius is the total distance between the points.
 * Status: tested on kattis:airlinehub
 */
#pragma once

db sphericalDistance(db f1, db t1,
		db f2, db t2, db radius) {
	db dx = sin(t2)*cos(f2) - sin(t1)*cos(f1);
	db dy = sin(t2)*sin(f2) - sin(t1)*sin(f1);
	db dz = cos(t2) - cos(t1);
	db d = sqrt(dx*dx + dy*dy + dz*dz);
	return radius*2*asin(d/2);
}