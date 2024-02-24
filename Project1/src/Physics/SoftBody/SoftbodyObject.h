#pragma once
#include "../../model.h"
#include "../PhysicsCollisionTypes.h"


struct Point
{
//public:
	Point() {};
	Point(glm::vec3 position, glm::vec3 oldPostiion,Vertex* vertex) : 
	position(position),previousPosition(oldPostiion), vertex(nullptr)
	{
	}

	glm::vec3 position = glm::vec3(0);
	glm::vec3 previousPosition = glm::vec3(0);
	Vertex* vertex = nullptr;
	bool locked = false;
};

struct Stick
{
//public:
	Stick() {};
	Point* pointA = nullptr;
	Point* pointB = nullptr;
	float restLength = 0;
	bool isActive = true;
};

class SoftbodyObject : public Model
{
public:
	SoftbodyObject();
	~SoftbodyObject();

	bool isSoftBodyActive = true;

	float acceleration = 5 /*-9.81f*/;


	std::vector<Triangle> listOfTriangles;
	std::vector<Point*> listOfPoints;
	std::vector<Stick*> listOfSticks;
	std::vector<Triangle> GetTriangleList();

	void Initialize();
	void CalculateTriangles();
	void CalculateVertex();


	void DrawProperties()override;
	void SceneDraw()override;

	void Start() override;
	void Update(float deltaTime) override;
	void Render() override;
	void OnDestroy() override;

	void UpdateVerlet(float deltaTime);
	void UpdateSticks(float deltaTime);
	void UpdatePoints(float deltaTime);

	void CollisionTest();
	void UpdateVertices();
	void UpdateNormals();

	void CleanZeros(glm::vec3& value);

private:
	float PointsDistance(Point* pointA, Point* pointB);

	bool isIntialised = false;
	float renderRadius = 0.0025f;

	glm::vec3 downVector = glm::vec3(0, -1, 0);
	const double MAX_DELTATIME = 1.0 / 60.0;
};

