#include <Sphere.h>

Sphere::Sphere(unsigned int xSegments, unsigned int ySegments)
{

    Vertex vertex;
    glm::vec3 vector;

    for (unsigned int y = 0; y <= ySegments; ++y)
    {
        for (unsigned int x = 0; x <= xSegments; ++x)
        {

            Vertex vertex;
            glm::vec3 vector;

            float xSegment = (float)x / (float)xSegments;
            float ySegment = (float)y / (float)ySegments;
            float xPos = std::cos(xSegment * TAU) * std::sin(ySegment * PI); // TAU is 2PI
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * TAU) * std::sin(ySegment * PI);


            vector.x = xPos;
            vector.y = yPos;
            vector.z = zPos;

            vertex.Position = vector;
            vertex.TexCoords = glm::vec2(xSegment, ySegment);
            vertex.Normal = vector;

            vertices.push_back(vertex);

        }
    }

    bool oddRow = false;
    for (int y = 0; y < ySegments; ++y)
    {
        for (int x = 0; x < xSegments; ++x)
        {
            Indices.push_back((y + 1) * (xSegments + 1) + x);
            Indices.push_back(y * (xSegments + 1) + x);
            Indices.push_back(y * (xSegments + 1) + x + 1);

            Indices.push_back((y + 1) * (xSegments + 1) + x);
            Indices.push_back(y * (xSegments + 1) + x + 1);
            Indices.push_back((y + 1) * (xSegments + 1) + x + 1);
        }
    }

    // You must implement this function!
    setupSphere();
}


void Sphere::setupSphere() {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO); //copy the indices into the buffer

    glBindVertexArray(VAO);

    //copy our vertices array in a buffer for OpenGL to use 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    //A bind and an unbind call (like VBO)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);
    
    //then set our vertex attributes pointers
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,sizeof(Vertex), NULL);
    glEnableVertexAttribArray (0);
}

void Sphere::Draw() {

    glBindVertexArray(VAO);
    //render the triangles in wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //render the triangles
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, NULL);
    
    glBindVertexArray(0);
}