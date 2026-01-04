#ifndef UFONT_BITMAP
#define UFONT_BITMAP

#define NOMINMAX

#include "outline.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>


namespace uf::detail {
	
	auto cccw = [](Coord A, Coord B, Coord C) {
		return (C.y - A.y) * (B.x - A.x) >= (B.y - A.y) * (C.x - A.x);
		};

	auto intersects = [&](Coord A, Coord B, Coord C, Coord D) {
		return cccw(A, C, D) != cccw(B, C, D) && cccw(A, B, C) != cccw(A, B, D);
	};

	bool inside(std::vector<std::vector<Coord>> const& outline, Coord point) {
		int modCount = 0;
		for (auto const& contour : outline) {
			for (int i = 0; i < contour.size() - 1; i++)
				if (intersects({ -100, point.y }, { point.x, point.y }, contour[i], contour[i + 1]))
					modCount += 1;

			if (intersects({ -100, point.y }, { point.x, point.y }, contour[contour.size() - 1], contour[0]))
				modCount += 1;
		}

		return modCount % 2 != 0;
	};


	double dist2(double x, double y, double x1, double y1, double x2, double y2) {
		double A = x - x1;
		double B = y - y1;
		double C = x2 - x1;
		double D = y2 - y1;

		double dot = A * C + B * D;
		double len_sq = C * C + D * D;
		double param = dot / len_sq;

		double xx, yy;

		if (param < 0) {
			xx = x1;
			yy = y1;
		}
		else if (param > 1) {
			xx = x2;
			yy = y2;
		}
		else {
			xx = x1 + param * C;
			yy = y1 + param * D;
		}

		auto chebyshev_distance = [](int r1, int c1, int r2, int c2) {
			// return the maximum of the absolute differences
			return max(std::abs(r1 - r2), std::abs(c1 - c2));
			};


		double dx = x - xx;
		double dy = y - yy;
		//return sqrt(dx * dx + dy * dy);
		return chebyshev_distance(x, y, xx, yy);
	}

	

	auto scale(std::vector<unsigned char> const& data, int w, int h, float scaleFactor) {
		auto getPixel = [](const std::vector<unsigned char> & image, int width, int x, int y) {
			// Check if coordinates are within the image bounds
			if (x >= 0 && x < width && y >= 0 && y < image.size() / width) {
				return image[y * width + x];
			}
			return (unsigned char)0;  // Return 0 for out of bounds
			};

		int newWidth = std::round(w * scaleFactor);
		int newHeight = std::round(h * scaleFactor);
		std::vector<unsigned char> result(newWidth * newHeight);

		for (int y = 0; y < newHeight; ++y) {
			for (int x = 0; x < newWidth; ++x) {
				// Calculate the original coordinates
				int origX = std::round(x / scaleFactor);
				int origY = std::round(y / scaleFactor);

				// Get the pixel intensity at the original coordinates
				unsigned char intensity = getPixel(data, w, origX, origY);

				// Set the pixel intensity in the result image
				result[y * newWidth + x] = intensity;
			}
		}

		return std::make_tuple(result, newWidth, newHeight);
	}

	auto chebyshev_distance = [](int r1, int c1, int r2, int c2) {
		// return the maximum of the absolute differences
		return max(std::abs(r1 - r2), std::abs(c1 - c2));
		};
	
	/*
	auto load_bitmap(FontCharacter const& c) {
		std::vector<unsigned char> data(c.width * c.height, unsigned char{ 0 });

		auto check_intersection_row = [&](std::vector<double>& iPoints, Coord a, Coord b, int y) {
			auto [x1, y1] = a;
			auto [x2, y2] = b;
		

			if (y1 == y2 && y1 == y) {
				iPoints.push_back(x1);
				iPoints.push_back(x2);		
			}
			else {
				if (y1 <= y && y2 >= y) {
					double x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
					iPoints.push_back(x);
				}
				if (y1 >= y && y2 <= y) {
					double x = x2 + (y - y2) * (x1 - x2) / (y1 - y2);
					iPoints.push_back(x);
				}
			}
		};

		auto each_line = [&](int y) {
			std::vector<double> iPoints;

			for (auto const& contour : c.outline) {
				check_intersection_row(iPoints, contour[contour.size() - 1], contour[0], y);

				for (int i = 0; i < contour.size() - 1; i++) {
					check_intersection_row(iPoints, contour[i], contour[i + 1], y);
				}

			}

			if (iPoints.empty())
				return;

			std::sort(iPoints.begin(), iPoints.end());

			for (int i = 0; i < iPoints.size() - 1; i += 2) {
				for (int x = iPoints[i]; x <= iPoints[i + 1]; x++) {
					if (x < 0 || x >= c.width)
						continue;

					data[y * c.width + x] = 255;
				}
			}
		};

		for (int j = 0; j < c.height; j++)
			each_line(j);

		// Perform aa on each pixel
		for (int i = 0; i < c.width - 1; i++) {
			for (int j = 0; j < c.height - 1; j++) {
				int total = 0;
				for (int m = -1; m <= 1; m++) {
					for (int n = -1; n <= 1; n++) {
						if (i + m < 0 || i + m >= c.width || j + n < 0 || j + n >= c.height)
							continue;
						int x = i + m, y = j + n ;
						total += data[y * c.width + x];
					}
				}
				data[j * c.width + i] = total / 9;
			}
		}


		return data;
	}
	*/

	void perform_aa_advanced(std::vector<unsigned char>& data, int width, int height, int c) {
		std::vector<int> integral_image(width * height);
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				integral_image[j * width + i] = data[j * width + i]
					+ (i > 0 ? integral_image[j * width + i - 1] : 0)
					+ (j > 0 ? integral_image[(j - 1) * width + i] : 0)
					- (i > 0 && j > 0 ? integral_image[(j - 1) * width + i - 1] : 0);
			}
		}

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (data[j * width + i] == 255)
					continue;

				int x1 = max(i - c, 0);
				int x2 = min(i + c, width - 1);
				int y1 = max(j - c, 0);
				int y2 = min(j + c, height - 1);

				int count = (x2 - x1 + 1) * (y2 - y1 + 1);
				int total = integral_image[y2 * width + x2]
					- (x1 > 0 ? integral_image[y2 * width + x1 - 1] : 0)
					- (y1 > 0 ? integral_image[(y1 - 1) * width + x2] : 0)
					+ (x1 > 0 && y1 > 0 ? integral_image[(y1 - 1) * width + x1 - 1] : 0);

				data[j * width + i] = total / count;
			}
		}
	}
	
	auto load_bitmap(FontCharacter const& c) {
		std::vector<unsigned char> data(c.width * c.height, unsigned char{ 0 });

		for (int i = 0; i < c.width; i++) {
			for (int j = 0; j < c.height; j++) {
				float x = i + 0.5, y = j + 0.5;
				data[j * c.width + i] = inside(.outline, Coord{ x, y }) ? 255 : 0;
			}
		}

		// Perform aa on each pixel
		perform_aa_advanced(data, c.width, c.height, 2);

		return std::make_tuple(data, c.width, c.height);
	}
	

	/*
	auto load_bitmap(FontCharacter const& c) {
		std::vector<unsigned char> data(c.width * c.height, unsigned char{ 0 });
		std::vector<int> distances(c.width * c.height, unsigned char{ 0 });

		double largestValue = 0.0;
		auto each_pixel = [&](float x, float y) {
			
			float largestDistance = FLT_MAX;

			//if (!inside(c.outline, Coord{ float(x), float(y) })) {
			//	distances[y * c.width + x] = 0.0;
			//	return;
			//}

			for (auto contour : c.outline) {
				for (int i = 0; i < contour.size() - 1; i++) {
					double dist = dist2(x + 0.5, y + 0.5, contour[i].x, contour[i].y, contour[i + 1].x, contour[i + 1].y);
					largestDistance = min(largestDistance, dist);
					distances[y * c.width + x] = largestDistance;
					largestValue = max(largestValue, largestDistance);
				}

				double dist = dist2(x + 0.5, y + 0.5, contour[contour.size() - 1].x, contour[contour.size() - 1].y, contour[0].x, contour[0].y);
				largestDistance = min(largestDistance, dist);
				distances[y * c.width + x] = largestDistance;
				largestValue = max(largestValue, largestDistance);
			}

			};

		for (float i = 0; i < c.width; i++) {
			for (float j = 0; j < c.height; j++) {
				each_pixel(i, j);
			}
		}

		float dist = chebyshev_distance(0, 0, c.width, c.height);

		for (float i = 0; i < c.width; i++) {
			for (float j = 0; j < c.height; j++) {
				data[j * c.width + i] = ((distances[j * c.width + i] - 0.0) / (dist - 0)) * 255.0;
			}
		}

		return data;
	}
	*/
	
	
	auto load_signed_bitmap(FontCharacter const& c, float cellCount, float gScale = 5.0) {
		auto [data, bw, bc] = load_bitmap(c);
		auto [scaledData, scaledW, scaledH] = scale(data, c.width, c.height, gScale);

		float cellU = c.width / cellCount, cellV = c.height / cellCount;
		auto cellCenter = [&](int cellX, int cellY) { return Coord{ (cellU * (float)cellX) + (cellU * 0.5f), (cellV * (float)cellY) + (cellV * 0.5f) };};

		float length = std::sqrt(scaledW * scaledW + scaledH * scaledH);

		// Loop through each pixel in the bitmap, find the closest distance value and if its less than 0 make it white
		std::vector<unsigned char> rs(scaledW * scaledH, 0);

		for (int y = 0; y < scaledH; y++) {
			for (int x = 0; x < scaledW; x++) {
				int cellX = x / cellU, cellY = y / cellV;
				auto cell_center = cellCenter(cellX, cellY);

				float pxCenter = x + 0.5, pyCenter = y + 0.5;

				auto outline = c.outline;
				for (auto& o : outline) {
					for (auto& [x, y] : o)
						x *= gScale, y *= gScale;
				}


				float smallestDist = FLT_MAX, largestDist = FLT_MIN;
				for (auto o : outline) {
					for (int i = 0; i < o.size() - 1; i++) {
						float dist = dist2(pxCenter, pyCenter, o[i].x, o[i].y, o[i + 1].x, o[i + 1].y);
						if (dist < smallestDist) {
							smallestDist = dist;
						}
						smallestDist = dist < smallestDist ? dist : smallestDist;
						largestDist = dist > largestDist ? dist : largestDist;
					}

					float dist = dist2(pxCenter, pyCenter, o[o.size() - 1].x, o[o.size() - 1].y, o[0].x, o[0].y);
					if (dist < smallestDist) {
						smallestDist = dist;
					}
					smallestDist = dist < smallestDist ? dist : smallestDist;
					largestDist = dist > largestDist ? dist : largestDist;
				}
				
				if (inside(outline, Coord{ pxCenter, pyCenter }))
					smallestDist *=  -1.0;

				
				if (std::floor(smallestDist) < 0.0)
					rs[y * scaledW + x] = 255;
				else if (std::floor(smallestDist) > length / gScale)
					rs[y * scaledW + x] = 0;
				else
					rs[y * scaledW + x] = (1.0 - (smallestDist / (length / gScale))) * 255;

				
		
			}
		}

		auto [rs2, sw1, sh1] = scale(rs, scaledW, scaledH, 1.0 / gScale);

		return std::make_tuple(rs2, sw1, sh1);
	}
}



#endif UFONT_BITMAP