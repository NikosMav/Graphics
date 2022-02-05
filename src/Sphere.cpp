#include <Sphere.h>

const float PI = 3.14159265359f;
const float TAU = 6.28318530717f;


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
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    // PROSOXH edwwwww TO 0
    //unsigned int vertexPositionID = 0;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray (0);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint), &Indices[0], GL_STATIC_DRAW);

    this->numsToDraw = Indices.size();
}

void Sphere::Draw() {

    // draw sphere
    glBindVertexArray(VAO);
    // glEnable(GL_PRIMITIVE_RESTART);
    // glPrimitiveRestartIndex(GL_PRIMITIVE_RESTART_FIXED_INDEX);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, numsToDraw, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}