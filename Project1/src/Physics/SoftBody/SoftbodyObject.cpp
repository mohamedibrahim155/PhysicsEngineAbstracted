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
			temp->sphere.center = vertex.Position;
			temp->sphere.radius = 0.025f;

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
			edge1->restLength = glm::distance(point1->position, point2->position);
			listOfSticks.push_back(edge1);

			Stick* edge2 = new Stick();
			edge2->pointA = point2;
			edge2->pointB = point3;
			edge2->restLength = glm::distance(point2->position, point3->position);
			listOfSticks.push_back(edge2);

			Stick* edge3 = new Stick();
			edge3->pointA = point3;
			edge3->pointB = point1;
			edge3->restLength = glm::distance(point3->position, point1->position);
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

	if (!ImGui::TreeNodeEx("Softbody properties", ImGuiTreeNodeFlags_DefaultOpen))
	{
		return;
	}

	ImGui::Text("Show Debug");
	ImGui::SameLine();
	ImGui::Checkbox("##ShowDebug" , &showDebug);
	
	ImGui::Text("tightnessFactor");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(150);
	ImGui::InputFloat("##tightnessFactor", &tightnessFactor,0,0.1,"%.2f");

	ImGui::NewLine();
	for (int i = 0; i < listOfPoints.size(); ++i)
	{
		Point*& point = listOfPoints[i];

		
		ImGui::Text("locked");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::Checkbox(("##locked" + std::to_string(i)).c_str(), &point->locked);

		ImGui::SetNextItemWidth(80);
		ImGui::Text("points");

		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat(("x##" + std::to_string(i)).c_str(), &point->position.x);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat(("y##" + std::to_string(i)).c_str(), &point->position.y);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat(("z##" + std::to_string(i)).c_str(), &point->position.z);


	}


	ImGui::TreePop();
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
	if (showDebug)
	{
		for (Point* point : listOfPoints)
		{
			GraphicsRender::GetInstance().DrawSphere(point->UpdateSphere(transform).center, point->UpdateSphere(transform).radius, glm::vec4(0, 1, 1, 1), true);
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
	UpdateSticks(deltaTime);

	UpdateVertices();
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

				//if (deltaLength != 0)
				{
					float diff = (deltaLength - stick->restLength) / deltaLength;

					if (diff > 0.1f)
					{
					///	stick->isActive = false;
					}
				
					if (!pointA->locked) pointA->position += delta * 0.5f * diff * tightnessFactor;
					
					if (!pointB->locked) pointB->position -= delta * 0.5f * diff * tightnessFactor;
				

					CleanZeros(pointA->position);
					CleanZeros(pointB->position);
				}


				//SEBASTIAN's
				/*glm::vec3 centre = (pointA->position + pointB->position) * 0.5f;

				glm::vec3 direction = glm::normalize(pointA->position - pointB->position);

				if (glm::length(direction)!=0)
				{
					if (!pointA->locked)
					{
						stick->pointA->position = centre + direction * (stick->restLength * 0.5f);
					}

					if (!pointB->locked)
					{
						stick->pointB->position = centre - direction * (stick->restLength *0.5f );
					}

				}*/


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

			/*glm::vec3 direction = currentPosition - prevPosition;

			
				point->position += (direction) + (glm::vec3(0, -acceleration, 0) * (float)(deltaTime * deltaTime));

				point->previousPosition = currentPosition;*/


				/*if (CheckSoftBodyAABBCollision(point,updateAABBTest->UpdateAABB()))
				{
					std::cout << "CollisionDetected" << std::endl;
					handleSoftBodyAABBCollision(*point, updateAABBTest->UpdateAABB());
				}
				else*/
				{
					point->position += (point->position - point->previousPosition);

					point->position += downVector * acceleration * (deltaTime * deltaTime);

				}


				point->previousPosition = currentPosition;

				CleanZeros(point->position);
				CleanZeros(point->previousPosition);
				
		}

	}

}

void SoftbodyObject::CollisionTest()
{
	//for (Point* point : listOfPoints)
	//{
	//	if (point->position.y < -5.0f)
	//	{
	//		point->position.y = -5.0f;
	//	}
	//}

	//for (Point* point : listOfPoints)
	//{
	//	/*if (CheckSoftBodyAABBCollision(point, updateAABBTest->UpdateAABB()))
	//	{
	//		handleSoftBodyAABBCollision(*point,updateAABBTest->UpdateAABB());
	//		std::cout << "Collision Detected" << std::endl;
	//	}*/
	////	cSphere sphere = updateAABBTest->UpdateSphere();
	//	//if (CheckSoftBodySphereCollision(point, sphere))
	//	{
	//		//std::cout << "sphere collission Detected" << std::endl;

	//		//glm::vec3 particleToCentreRay = point->position - updateAABBTest->UpdateSphere().center;
	//		//// Normalize to get the direction
	//		//particleToCentreRay = glm::normalize(particleToCentreRay);


	//		////if (glm::length(particleToCentreRay) != 0)
	//		//{
	//		//	point->position = (particleToCentreRay * updateAABBTest->UpdateSphere().radius) + updateAABBTest->UpdateSphere().center;
	//		//}
	//		std::cout << "Sphere collisiondetected" << std::endl;
	//	//	HandleSoftBodySphereCollision(point, sphere);
	//	}
	//}

	

	//for (Point* point : listOfPoints)
	//{
	//	glm::vec3 sphereCentre = glm::vec3(0.0f, -2, 0);
	//	float sphereRadius = 1;

	//	float distanceToSphere = glm::distance(point->position,
	//		sphereCentre);
	//	if (distanceToSphere < sphereRadius)
	//	{
	//		// it's 'inside' the sphere
	//		// Shift or slide the point along the ray from the centre of the sphere
	//		glm::vec3 particleToCentreRay = point->position - sphereCentre;
	//		// Normalize to get the direction
	//		particleToCentreRay = glm::normalize(particleToCentreRay);


	//		if (glm::length(particleToCentreRay)!=0)
	//		{
	//			point->position = (particleToCentreRay * sphereRadius) + sphereCentre;
	//		}
	//		
	//	

	//	}
	//}

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

	//for (Point* point : listOfPoints)
	//{
	//	point->vertex->Position = point->position;


	//	glm::vec4 vertex2Matrix = glm::vec4(point->position, 1.0f);
	//	glm::mat4 modelmatrix = transform.GetModelMatrix();

	//	vertex2Matrix = modelmatrix * vertex2Matrix;

	//	//vertex2Matrix = transform.GetModelMatrix() * vertex2Matrix;


	//	point->vertex->Position = glm::vec3(vertex2Matrix.x, vertex2Matrix.y, vertex2Matrix.z);
	//	/*point->vertex->Position.y = vertex2Matrix.y;
	//	point->vertex->Position.z = vertex2Matrix.z;*/
	//}

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

bool SoftbodyObject::CheckSoftBodyAABBCollision(Point* particle, const cAABB& aabb)
{

	return (
		particle->position.x >= aabb.minV.x && particle->position.x <= aabb.maxV.x &&
		particle->position.y >= aabb.minV.y && particle->position.y <= aabb.maxV.y &&
		particle->position.z >= aabb.minV.z && particle->position.z <= aabb.maxV.z
		);
	
}

bool SoftbodyObject::CheckSoftBodySphereCollision(Point* point, const cSphere& sphere)
{

	float distance = glm::distance(point->position,	sphere.center);

	return distance < sphere.radius;
}

void SoftbodyObject::HandleSoftBodySphereCollision(Point*& point, const cSphere& sphere)
{
	//float distance = glm::distance(point->position, sphere.center);

	//if (distance < sphere.radius)
	{
		glm::vec3 particleToCentreRay = point->position - sphere.center;
		// Normalize to get the direction
		particleToCentreRay = glm::normalize(particleToCentreRay);


		if (glm::length(particleToCentreRay) != 0)
		{
			point->position = (particleToCentreRay * sphere.radius) + sphere.center;
		}
	}
	

	//return;
}

void SoftbodyObject::handleSoftBodyAABBCollision(Point& point,const cAABB& aabb)
{
	//
		/*if (point.position.x < aabb.minV.x)
		{
			point.position.x = aabb.minV.x;
		}
		else if (point.position.x > aabb.maxV.x) 
		{
			point.position.x = aabb.maxV.x;
		}

		if (point.position.y < aabb.minV.y) 
		{
			point.position.y = aabb.minV.y;
		}
		else if (point.position.y > aabb.maxV.y) 
		{
			point.position.y = aabb.maxV.y;
		}

		if (point.position.z < aabb.minV.z) 
		{
			point.position.z = aabb.minV.z;
		}
		else if (point.position.z > aabb.maxV.z) 
		{
			point.position.z = aabb.maxV.z;
		}*/

	point.position.x = glm::clamp(point.position.x, aabb.minV.x, aabb.maxV.x);
	point.position.y = glm::clamp(point.position.y, aabb.minV.y, aabb.maxV.y);
	point.position.z = glm::clamp(point.position.z, aabb.minV.z, aabb.maxV.z);

	//point.position.y = glm::clamp(point.position.y, aabb.minV.y, aabb.maxV.y);
		// 
		// point.position = glm::vec
		
}
