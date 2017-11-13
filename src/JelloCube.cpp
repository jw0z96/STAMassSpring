#include "JelloCube.h"

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------

JelloCube::JelloCube()
{
	std::cout<<"creating jello cube!\n";
	generate();
}

JelloCube::~JelloCube()
{
	std::cout<<"bye!\n";
}

void JelloCube::generate()
{
	int sx = 5;
	int sy = 5;
	int sz = 5;

	ngl::Vec3 topRight = ngl::Vec3(1.0, 1.0, 1.0);
	ngl::Vec3 bottomLeft = ngl::Vec3(0.0, 0.0, 0.0);

	ngl::Vec3 span = topRight - bottomLeft;
	ngl::Vec3 step = span / ngl::Vec3(sx, sy, sz);

	// populate the array of masses
	for (int i = 0; i < sx; ++i)
	{
		for (int j = 0; j < sy; ++j)
		{
			for (int k = 0; k < sz; ++k)
			{
				ngl::Vec3 pt = ngl::Vec3(i, j, k) * step + bottomLeft;
				m_massPoints.emplace_back(new ngl::Vec3(pt));
			}
		}
	}

	// helper lambda to get the index into a 3d grid
	auto getIndex = [](int x, int y, int z, int tx, int ty) {return (z * tx * ty) + (y * tx) + x;};

	// populate the arrays of springs
	for (int i = 0; i < sx; ++i)
	{
		for (int j = 0; j < sy; ++j)
		{
			for (int k = 0; k < sz; ++k)
			{
				// the index at (i, j, k)
				int currentIndex = getIndex(i, j, k, sx, sy);

				// create structural springs
				if ((i + 1) < sx) // connect (i,j,k) to (i+1, j, k)
					m_structuralSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j, k, sx, sy)]);
				if ((j +1) < sy) // connect (i,j,k) to (i, j+1, k)
					m_structuralSprings.emplace_back(Spring(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 1, k, sx, sy)]));
				if ((k +1) < sz) // connect (i,j,k) to (i, j, k+1)
					m_structuralSprings.emplace_back(Spring(m_massPoints[currentIndex], m_massPoints[getIndex(i, j, k + 1, sx, sy)]));

				// create bend springs
				if ((i + 2) < sx) // connect (i,j,k) to (i+2, j, k)
					m_bendSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 2, j, k, sx, sy)]);
				if ((j +2) < sy) // connect (i,j,k) to (i, j+2, k)
					m_bendSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 2, k, sx, sy)]);
				if ((k +2) < sz) // connect (i,j,k) to (i, j, k+2)
					m_bendSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j, k + 2, sx, sy)]);

				// create shear springs
				if ((i + 1) < sx && (j + 1) < sy) // connect (i,j,k) to (i+1,j+1,k)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j + 1, k, sx, sy)]);

				if ((i + 1) < sx && (k + 1) < sz) // connect (i,j,k) to (i+1,j,k+1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j, k + 1, sx, sy)]);

				if ((i + 1) < sx && k > 0) // connect (i,j,k) to (i+1,j,k-1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j, k - 1, sx, sy)]);

				if ((i + 1) < sx && j > 0) // connect (i,j,k) to (i+1,j-1,k)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j - 1, k, sx, sy)]);

				if ((j + 1) < sy && (k + 1) < sz) // connect (i,j,k) to (i,j+1,k+1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 1, k + 1, sx, sy)]);

				if ((j + 1) < sy && k > 0) // connect (i,j,k) to (i,j+1,k-1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 1, k - 1, sx, sy)]);

				if (j > 0 && (k + 1) < sz) // connect (i,j,k) to (i,j-1,k+1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j - 1, k + 1, sx, sy)]);

				if (j > 0 && k > 0) // connect (i,j,k) to (i,j-1,k-1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j - 1, k - 1, sx, sy)]);

				if (i > 0 && (j + 1) < sy) // connect (i,j,k) to (i-1,j+1,k)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j + 1, k, sx, sy)]);

				if (i > 0 && (k + 1) < sz) // connect (i,j,k) to (i-1,j,k+1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j, k + 1, sx, sy)]);

				if (i > 0 && k > 0) // connect (i,j,k) to (i-1,j,k-1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j, k - 1, sx, sy)]);

				if (i > 0 && j > 0) // connect (i,j,k) to (i-1,j-1,k)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j - 1, k, sx, sy)]);
			}
		}
	}
}

void JelloCube::update()
{
	for (size_t i = 0; i < m_massPoints.size(); ++i)
	{
		*(m_massPoints[i]) += ngl::Vec3(0.0, 0.0001 * sin(i), 0.0);
	}
}
