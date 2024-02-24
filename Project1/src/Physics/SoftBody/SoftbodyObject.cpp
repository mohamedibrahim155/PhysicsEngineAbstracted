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
		

		for (size_t index = 0; index < mesh->vertices.size(); index++)
		{
			glm::vec4 vertexMatrix = glm::vec4(mesh->vertices[index].Position.x,
				mesh->vertices[index].Position.y,
				mesh->vertices[index].Position.z, 1.0f);

			vertexMatrix = transform.GetModelMatrix() * vertexMatrix;


			mesh->vertices[index].Position.x = vertexMatrix.x;
			mesh->vertices[index].Position.y = vertexMatrix.y;
			mesh->vertices[index].Position.z = vertexMatrix.z;


		}



		listOfPoints.reserve(mesh->vertices.size());

		for (Vertex& vertex : mesh->vertices)
		{
			Point* temp = new Point();

			temp->position = vertex.Position;
			temp->previousPosition = temp->position;
			temp->vertex = &vertex;

			listOfPoints.push_back(temp);
		}



		for (size_t i = 0; i < mesh->indices.size(); i += 3)
		{
			Triangle tempTri;

			tempTri.v1 = mesh->vertices[mesh->indices[i]].Position;
			tempTri.v2 = mesh->vertices[mesh->indices[i + 1]].Position;
			tempTri.v3 = mesh->vertices[mesh->indices[i + 2]].Position;

			tempTri.normal = (mesh->vertices[mesh->indices[i]].Normal +
				mesh->vertices[mesh->indices[i + 1]].Normal +
				mesh->vertices[mesh->indices[i + 2]].Normal) / 3.0f;
		
			tempTri.CalculateMidpoint();
			listOfTriangles.push_back(tempTri);



			Point* point1 = listOfPoints[mesh->indices[i]];
			Point* point2 = listOfPoints[mesh->indices[i +1]];
			Point* point3 = listOfPoints[mesh->indices[i + 2]];

			Stick* edge1 = new Stick();
			edge1->pointA = point1;
			edge1->pointB = point2;
			edge1->restLength = glm::distance(edge1->pointA->position , edge1->pointB->position);
			listOfSticks.push_back(edge1);

			Stick* edge2 = new Stick();
			edge2->pointA = point2;
			edge2->pointB = point3;
			edge2->restLength = glm::distance(edge2->pointA->position, edge2->pointB->position);
			listOfSticks.push_back(edge2);

			Stick* edge3 = new Stick();
			edge3->pointA = point3;
			edge3->pointB = point1;
			edge2->restLength = glm::distance(edge3->pointA->position, edge3->pointB->position);
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

	for (Point* point : listOfPoints)
	{
		ImGui::SetNextItemWidth(80);
		ImGui::Text("points");

		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("x", &point->position.x);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("y", &point->position.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("z", &point->position.x);
	}
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
	//UpdateVerlet(deltaTime);  // disable as of now
}

void SoftbodyObject::Render()
{
	if (isIntialised)
	{
		for (Point* point : listOfPoints)
		{
			GraphicsRender::GetInstance().DrawSphere(point->position, renderRadius, glm::vec4(0, 1, 1, 1), true);
		}

		for (Stick* stick: listOfSticks )
		{
			GraphicsRender::GetInstance().DrawLine(stick->pointA->position, stick->pointB->position, glm::vec4(1, 1, 0, 1));

		}
	}
	
}

void SoftbodyObject::OnDestroy()
{
}

void SoftbodyObject::UpdateVerlet(float deltaTime)
{
	
	UpdatePoints(deltaTime);

	CollisionTest();

	//UpdateSticks(deltaTime);
	//UpdateVertices();
}

void SoftbodyObject::UpdateSticks(float deltaTime)
{
	const unsigned int MAX_ITERATION = 10;

	for (size_t i = 0; i < MAX_ITERATION; i++)
	{
		for (Stick* stick : listOfSticks)
		{
			if (stick->isActive)
			{

				Point* pointA = stick->pointA;
				Point* pointB = stick->pointB;
				//// FEENEYS's
				glm::vec3 delta = pointB->position - pointA->position;
				float deltaLength = glm::length(delta);

				if (deltaLength != 0)
				{
					float diff = (deltaLength - stick->restLength) / deltaLength;

					if (diff > 0.01f)
					{
						stick->isActive = false;
					}
					const float tightnessFactor = 1.0f;

					pointA->position += delta * 0.5f * diff * tightnessFactor;
					pointB->position -= delta * 0.5f * diff * tightnessFactor;

					CleanZeros(pointA->position);
					CleanZeros(pointB->position);
				}


				//SEBASTIAN's
				//glm::vec3 centre = (pointA->position + pointB->position) * 0.5f;

				//glm::vec3 direction = glm::normalize(pointA->position - pointB->position);

				//if (glm::length(direction)!=0)
				//{
				//	if (!pointA->locked)
				//	{
				//		stick->pointA->position = centre + direction * (stick->restLength * 0.5f);
				//	}

				//	if (!pointB->locked)
				//	{
				//		stick->pointB->position = centre - direction * (stick->restLength *0.5f );
				//	}

				//}


			}
		
		}
	}

}

void SoftbodyObject::UpdatePoints(float deltaTime)
{
	if (deltaTime > MAX_DELTATIME)
	{
		deltaTime = MAX_DELTATIME;
	}

	for (Point* point : listOfPoints)
	{
		if (!point->locked)
		{
			glm::vec3 currentPosition = point->position;
			glm::vec3 prevPosition = point->previousPosition;

			glm::vec3 direction = currentPosition - prevPosition;

			
				point->position += (direction) + (glm::vec3(0, -acceleration, 0) * (float)(deltaTime * deltaTime));

				point->previousPosition = currentPosition;

				/*point->position += (point->position - point->previousPosition);

				point->position += downVector * acceleration * (deltaTime * deltaTime);

				point->previousPosition = currentPosition;*/

				CleanZeros(point->position);
				CleanZeros(point->previousPosition);
				
		}

	}

}

void SoftbodyObject::CollisionTest()
{
	for (Point* point : listOfPoints)
	{
		if (point->position.y < -5.0f)
		{
			point->position.y = -5.0f;
		}
	}

//	return;

}

void SoftbodyObject::CleanZeros(glm::vec3& value)
{
	// 1.192092896e–07F 
	const float minFloat = 1.192092896e-07f;
	if ((value.x < minFloat) && (value.x > -minFloat))
	{
		value.x = 0.0f;
	}
	if ((value.y < minFloat) && (value.y > -minFloat))
	{
		value.y = 0.0f;
	}
	if ((value.z < minFloat) && (value.z > -minFloat))
	{
		value.z = 0.0f;
	}
}

void SoftbodyObject::UpdateVertices()
{
	for (Point* point : listOfPoints)
	{
		point->vertex->Position = point->position;
	}

	//UpdateNormals();

	for (std::shared_ptr<Mesh> mesh: meshes)
	{
		mesh->UpdateVertices();
	}
}

void SoftbodyObject::UpdateNormals()
{
	for (std::shared_ptr<Mesh> mesh : meshes)
	{

		for (size_t i = 0; i < mesh->vertices.size(); i++)
		{
			mesh->vertices[i].Normal = glm::vec3(0);
		}
	}

	for (std::shared_ptr<Mesh> mesh : meshes)
	{
		for (size_t i = 0; i < mesh->triangle.size(); i++)
		{
			unsigned int vertAIndex = mesh->indices[i + 0];
			unsigned int vertBIndex = mesh->indices[i + 1];
			unsigned int vertCIndex = mesh->indices[i + 2];


			Vertex& vertexA = mesh->vertices[vertAIndex];
			Vertex& vertexB = mesh->vertices[vertBIndex];
			Vertex& vertexC = mesh->vertices[vertCIndex];

			glm::vec3 vertA = vertexA.Position;
			glm::vec3 vertB = vertexB.Position;
			glm::vec3 vertC = vertexC.Position;

			glm::vec3 triangleEdgeAtoB = vertB - vertA;
			glm::vec3 triangleEdgeAtoC = vertC - vertA;


			glm::vec3 normal = glm::normalize(glm::cross(triangleEdgeAtoB, triangleEdgeAtoC));

			//normal = glm::normalize(normal);

			if (glm::length(normal)!=0)
			{
				vertexA.Normal.x += normal.x;
				vertexA.Normal.y += normal.y;
				vertexA.Normal.z += normal.z;

				vertexB.Normal.x += normal.x;
				vertexB.Normal.y += normal.y;
				vertexB.Normal.z += normal.z;

				vertexC.Normal.x += normal.x;
				vertexC.Normal.y += normal.y;
				vertexC.Normal.z += normal.z;
			}

			

		}
		
	}


	for (std::shared_ptr<Mesh> mesh : meshes)
	{

		/*for (size_t i = 0; i < mesh->vertices.size(); i++)
		{
			glm::vec3 normal = glm::vec3(mesh->vertices[i].Normal.x,
				mesh->vertices[i].Normal.y,
				mesh->vertices[i].Normal.z);

			normal = glm::normalize(normal);

			if (glm::length(normal) != 0)
			{
				mesh->vertices[i].Normal.x = normal.x;
				mesh->vertices[i].Normal.y = normal.y;
				mesh->vertices[i].Normal.z = normal.z;
			}
			

		}*/
	}
	

}

float SoftbodyObject::PointsDistance(Point* pointA, Point* pointB)
{
	return glm::distance(pointA->position, pointB->position);
}
