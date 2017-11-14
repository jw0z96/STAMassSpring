#include "JelloCube.h"

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------

JelloCube::JelloCube(double _k, double _damping) : m_k(_k), m_damping(_damping)
{
	std::cout<<"creating jello cube!\n";
	m_timestep = 0.1;
	m_t = 0.0;
	generate();
}

JelloCube::~JelloCube()
{
	std::cout<<"bye!\n";
}

void JelloCube::reset()
{
	m_t = 0.0;
	m_massPoints.clear();
	m_structuralSprings.clear();
	m_bendSprings.clear();
	m_shearSprings.clear();
	generate();
}

void JelloCube::setK(double _k)
{
	m_k = _k;

	for (size_t i = 0; i < m_structuralSprings.size(); ++i)
	{
		m_structuralSprings[i].setK(_k);
	}
	for (size_t i = 0; i < m_bendSprings.size(); ++i)
	{
		m_structuralSprings[i].setK(_k);
	}
	for (size_t i = 0; i < m_shearSprings.size(); ++i)
	{
		m_structuralSprings[i].setK(_k);
	}
}

void JelloCube::setDamping(double _damping)
{
	m_damping = _damping;

	for (size_t i = 0; i < m_structuralSprings.size(); ++i)
	{
		m_structuralSprings[i].setDamping(_damping);
	}
	for (size_t i = 0; i < m_bendSprings.size(); ++i)
	{
		m_structuralSprings[i].setDamping(_damping);
	}
	for (size_t i = 0; i < m_shearSprings.size(); ++i)
	{
		m_structuralSprings[i].setDamping(_damping);
	}
}

void JelloCube::generate()
{
	int sx = 5;
	int sy = 5;
	int sz = 5;

	ngl::Vec3 topRight = ngl::Vec3(0.5, 0.5, 0.5);
	ngl::Vec3 bottomLeft = ngl::Vec3(-0.5, -0.5, -0.5);

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
					m_structuralSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j, k, sx, sy)], m_k, m_damping);
				if ((j +1) < sy) // connect (i,j,k) to (i, j+1, k)
					m_structuralSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 1, k, sx, sy)], m_k, m_damping);
				if ((k +1) < sz) // connect (i,j,k) to (i, j, k+1)
					m_structuralSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j, k + 1, sx, sy)], m_k, m_damping);

				// create bend springs
				if ((i + 2) < sx) // connect (i,j,k) to (i+2, j, k)
					m_bendSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 2, j, k, sx, sy)], m_k, m_damping);
				if ((j +2) < sy) // connect (i,j,k) to (i, j+2, k)
					m_bendSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 2, k, sx, sy)], m_k, m_damping);
				if ((k +2) < sz) // connect (i,j,k) to (i, j, k+2)
					m_bendSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j, k + 2, sx, sy)], m_k, m_damping);

				// create shear springs
				if ((i + 1) < sx && (j + 1) < sy) // connect (i,j,k) to (i+1,j+1,k)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j + 1, k, sx, sy)], m_k, m_damping);

				if ((i + 1) < sx && (k + 1) < sz) // connect (i,j,k) to (i+1,j,k+1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j, k + 1, sx, sy)], m_k, m_damping);

				if ((i + 1) < sx && k > 0) // connect (i,j,k) to (i+1,j,k-1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j, k - 1, sx, sy)], m_k, m_damping);

				if ((i + 1) < sx && j > 0) // connect (i,j,k) to (i+1,j-1,k)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j - 1, k, sx, sy)], m_k, m_damping);

				if ((j + 1) < sy && (k + 1) < sz) // connect (i,j,k) to (i,j+1,k+1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 1, k + 1, sx, sy)], m_k, m_damping);

				if ((j + 1) < sy && k > 0) // connect (i,j,k) to (i,j+1,k-1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 1, k - 1, sx, sy)], m_k, m_damping);

				// if (j > 0 && (k + 1) < sz) // connect (i,j,k) to (i,j-1,k+1)
					// m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j - 1, k + 1, sx, sy)]);

				// if (j > 0 && k > 0) // connect (i,j,k) to (i,j-1,k-1)
				// 	m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j - 1, k - 1, sx, sy)]);

				// if (i > 0 && (j + 1) < sy) // connect (i,j,k) to (i-1,j+1,k)
				// 	m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j + 1, k, sx, sy)]);

				// if (i > 0 && (k + 1) < sz) // connect (i,j,k) to (i-1,j,k+1)
				// 	m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j, k + 1, sx, sy)]);

				// if (i > 0 && k > 0) // connect (i,j,k) to (i-1,j,k-1)
				// 	m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j, k - 1, sx, sy)]);

				// if (i > 0 && j > 0) // connect (i,j,k) to (i-1,j-1,k)
				// 	m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j - 1, k, sx, sy)]);
			}
		}
	}
}

void JelloCube::update()
{

	for (size_t i = 0; i < m_structuralSprings.size(); ++i)
	{
		m_structuralSprings[i].integrate(m_t, m_timestep);
	}

	for (size_t i = 0; i < m_bendSprings.size(); ++i)
	{
		m_bendSprings[i].integrate(m_t, m_timestep);
	}

	for (size_t i = 0; i < m_shearSprings.size(); ++i)
	{
		m_shearSprings[i].integrate(m_t, m_timestep);
	}

	for (size_t i = 0; i < m_structuralSprings.size(); ++i)
	{
		m_structuralSprings[i].update();
	}

	for (size_t i = 0; i < m_bendSprings.size(); ++i)
	{
		m_bendSprings[i].update();
	}

	for (size_t i = 0; i < m_shearSprings.size(); ++i)
	{
		m_shearSprings[i].update();
	}

	// update the timestep for the next time
	m_t += m_timestep;


	// for (size_t i = 0; i < m_massPoints.size(); ++i)
	// {
	// 	*(m_massPoints[i]) += ngl::Vec3(-0.001 * sin(m_t + i), 0.001 * sin(m_t + i), 0.001 * cos(m_t + i));
	// 	// *(m_massPoints[i]) += ngl::Vec3(0.0);
	// }

	float intensity = 0.01;
	*(m_massPoints[int(m_t + 20)%124]) += ngl::Vec3(intensity * sin(m_t + 49.0), intensity * sin(m_t), intensity * cos(m_t + 7.0));

}
