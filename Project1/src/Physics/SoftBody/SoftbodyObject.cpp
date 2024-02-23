#include "SoftbodyObject.h"
#include "../PhysicsEngine.h"
SoftbodyObject::SoftbodyObject()
{
	name = "Softbody";
}

SoftbodyObject::~SoftbodyObject()
{
	listOfTriangles.clear();
}

std::vector<Triangle> SoftbodyObject::GetTriangleList()
{
    return listOfTriangles;
}

void SoftbodyObject::Initialize()
{
	//CalculateTriangles();
	CalculateVertex();
	isIntialised = true;

	PhysicsEngine::GetInstance().AddSoftBodyObject(this);

}

void SoftbodyObject::CalculateTriangles()
{
	listOfTriangles.clear();

	for (std::shared_ptr<Mesh> mesh : meshes)
	{
		std::vector<Triangle> trianglelist;
		//std::vector<cSphere*> meshSphers;

		trianglelist.reserve(mesh->triangle.size());
		//meshSphers.reserve(mesh->triangle.size());

		for (const Triangles& triangle : mesh->triangle)
		{
			Triangle temp;
			temp.v1 = triangle.v1;
			temp.v2 = triangle.v2;
			temp.v3 = triangle.v3;
			temp.normal = triangle.normal;
			temp.CalculateMidpoint();

			//glm::vec3 sphereCenter = (temp.v1 + temp.v2 + temp.v3) / 3.0f;
			//float radius = glm::max(glm::distance(sphereCenter, temp.v1),
			//	glm::max(glm::distance(sphereCenter, temp.v2), glm::distance(sphereCenter, temp.v3)));

			listOfTriangles.push_back(temp);
			/*triangleSpheres.push_back(new cSphere(sphereCenter, radius));*/
		}

	}
}

void SoftbodyObject::CalculateVertex()
{
	listOfPoints.clear();

	for (std::shared_ptr<Mesh> mesh : meshes)
	{
		//std::vector<Point*> particleList;

		listOfPoints.reserve(mesh->vertices.size());

		for (Vertex vertex : mesh->vertices)
		{
			Point* temp = new Point();

			temp->position = vertex.Position;
			temp->previousPosition = temp->position;
			temp->vertex = &vertex;

			listOfPoints.push_back(temp);
		}



		for (size_t i = 0; i < mesh->indices.size(); i+=3)
		{
			Triangle tempTri;

			tempTri.v1 = mesh->vertices[mesh->indices[i]].Position;
			tempTri.v2 = mesh->vertices[mesh->indices[i + 1]].Position;
			tempTri.v3 = mesh->vertices[mesh->indices[i + 2]].Position;

			tempTri.normal = (mesh->vertices[mesh->indices[i]].Normal +
				mesh->vertices[mesh->indices[i + 1]].Normal +
				mesh->vertices[mesh->indices[i + 2]].Normal) / 3.0f;
		
			tempTri.CalculateMidpoint();

			Point* point1 = listOfPoints[mesh->indices[i]];
			Point* point2 = listOfPoints[mesh->indices[i +1]];
			Point* point3 = listOfPoints[mesh->indices[i + 2]];

			Stick* edge1 = new Stick();
			edge1->pointA = point1;
			edge1->pointB = point2;
			edge1->restLength = PointsDistance(point1, point2);

			Stick* edge2 = new Stick();
			edge2->pointA = point2;
			edge2->pointB = point3;
			edge2->restLength = PointsDistance(point2, point3);

			Stick* edge3 = new Stick();
			edge3->pointA = point3;
			edge3->pointB = point1;
			edge2->restLength = PointsDistance(point3, point1);


			listOfTriangles.push_back(tempTri);

			listOfSticks.push_back(edge1);
			listOfSticks.push_back(edge2);
			listOfSticks.push_back(edge3);

		}

		/*for (const Triangles& triangle : mesh->triangle)
		{
			Triangle temp;
			temp.v1 = triangle.v1;
			temp.v2 = triangle.v2;
			temp.v3 = triangle.v3;
			temp.normal = triangle.normal;
			temp.CalculateMidpoint();

			Stick* edge1 = new Stick();
			edge1->pointA = new Point(temp.v1, temp.v1, new Vertex());
			edge1->pointB = new Point(temp.v2, temp.v2, new Vertex());

			Stick* edge2 = new Stick();
			edge2->pointA = new Point(temp.v2, temp.v2, new Vertex());
			edge2->pointB = new Point(temp.v3, temp.v3, new Vertex());

			Stick* edge3 = new Stick();
			edge3->pointA = new Point(temp.v3, temp.v3, new Vertex());
			edge3->pointB = new Point(temp.v1, temp.v1, new Vertex());




			listOfTriangles.push_back(temp);
		}*/

	}
}

void SoftbodyObject::DrawProperties()
{
    Model::DrawProperties();
}

void SoftbodyObject::SceneDraw()
{
    Model::SceneDraw();
}

void SoftbodyObject::Start()
{
}

void SoftbodyObject::Update(float deltaTime)
{
}

void SoftbodyObject::Render()
{
	if (isIntialised)
	{
		for (Point* point : listOfPoints)
		{
			GraphicsRender::GetInstance().DrawSphere(point->position, 0.01f, glm::vec4(0, 1, 1, 1), true);
		}
	}
	
}

void SoftbodyObject::OnDestroy()
{
}

float SoftbodyObject::PointsDistance(Point* pointA, Point* pointB)
{
	return glm::distance(pointA->position, pointB->position);
}
