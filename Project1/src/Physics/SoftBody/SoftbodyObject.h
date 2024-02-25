#pragma once
#include "../../model.h"
#include "../PhysicsCollisionTypes.h"
#include"../PhysicsObject.h"

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
	cSphere sphere;

	cSphere UpdateSphere(const Transform& transform)
	{
		glm::vec3 originalCenter = sphere.center;
		float orginalRadius = sphere.radius;
		glm::mat4 transformMatrix = transform.GetModelMatrix();
		glm::vec4 transformedCenter = transformMatrix * glm::vec4(originalCenter, 1.0f);

		glm::mat4 scaleMatrix = glm::mat4(1.0f); // Initialize the scale matrix
		scaleMatrix[0][0] = transform.scale.x;
		scaleMatrix[1][1] = transform.scale.y;
		scaleMatrix[2][2] = transform.scale.z;

		float maxScale = glm::max(transform.scale.x, glm::max(transform.scale.y, transform.scale.z));
		float updatedRadius = orginalRadius * maxScale;

		return cSphere(glm::vec3(transformedCenter), updatedRadius);
	}
};

struct Stick
{
	Stick() {};
	Stick(Point* _pointA, Point* _pointB) : pointA(_pointA), pointB(_pointB)
	{
		restLength = glm::distance(_pointA->position, _pointB->position);
	};

	Point* pointA = nullptr;
	Point* pointB = nullptr;
	float restLength = 0;
	bool isActive = true;
	bool isLocked = false;
};

class SoftbodyObject : public Model
{
public:
	SoftbodyObject();
	~SoftbodyObject();

	bool isSoftBodyActive = true;

	float gravity = 0.1f /*-9.81f*/;


	std::vector<Triangle> listOfTriangles;
	std::vector<Point*> listOfPoints;
	std::vector<Stick*> listOfSticks;
	std::vector<Triangle> GetTriangleList();

	void Initialize();
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
	void AddLockSphere(glm::vec3 centre, float radius);

	PhysicsObject* updateAABBTest = nullptr;
private:

	bool showDebug = true;
	float renderRadius = 0.025f;
	float tightnessFactor = 1;

	glm::vec3 downVector = glm::vec3(0, -1, 0);
	const double MAX_DELTATIME = 1.0 / 60.0;

	bool CheckSoftBodyAABBCollision(Point* point, const cAABB& aabb);
	bool CheckSoftBodySphereCollision(Point* point, const cSphere& sphere);
	void HandleSoftBodySphereCollision(Point*& point, const cSphere& sphere);
	//bool CheckSoftBodyAABBCollision(const cAABB& aabb);

	void handleSoftBodyAABBCollision(Point& particle, const cAABB& aabb);

	bool IsLocked(Point* point, glm::vec3 centre, float radius);

	glm::vec3 lockSphereCenter;
	float lockRadius;
};

