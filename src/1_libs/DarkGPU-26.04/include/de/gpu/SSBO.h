#pragma once

#if 0
/*
4. Debug Output (GL_ARB_debug_output) – Nie wieder glGetError()!

Wer vor OpenGL 4.3 Fehler gesucht hat, musste nach jedem Befehl glGetError() aufrufen, um zu sehen, ob etwas schiefgelaufen ist. Man bekam nur einen kryptischen Fehlercode (wie 0x0501) und wusste nie, in welcher Zeile der Fehler lag.
Der Lebensretter in 4.3:
OpenGL 4.3 führt einen nativen Debug-Callback ein. Der Treiber ruft bei einem Fehler (oder einer Performance-Warnung) direkt eine C++-Funktion von dir auf und liefert eine lesbare Textnachricht, die Fehlerquelle und die exakte ID mit:
cpp
glEnable(GL_DEBUG_OUTPUT);
glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
                          GLsizei length, const GLchar* message, const void* userParam) {
    std::cerr << "[OpenGL Debug " << id << "] " << message << std::endl;
    // Hier kann man einen Breakpoint setzen, um den exakten C++ Callstack zu sehen!
}, nullptr);
Verwende Code mit Vorsicht.

5. Texture Views (glTextureView) – Texturen recyceln ohne Kopieren

Manchmal möchte man dieselben Texturdaten auf unterschiedliche Weise im Shader interpretieren. Ein klassisches Beispiel ist eine Texture-2D-Array-Shadowmap, bei der man im Geometrie-Shader das gesamte Array braucht, aber im Kaskaden-Schatten-Pass nur Zugriff auf eine einzelne Schicht (Layer) benötigt.
Mit glTextureView kannst du eine neue Textur-ID erstellen, die keinen eigenen Grafikspeicher verbraucht, sondern wie eine "Schnittmaske" (View) auf eine bereits existierende Textur verweist. Du kannst damit das Format ändern (z.B. RGBA-Daten plötzlich als R32-Ganzzahlen interpretieren) oder eben nur bestimmte Mipmaps oder Layer einer Textur isolieren, ohne ein einziges Byte auf der GPU zu kopieren.

6. Program Pipeline Objects (glGenProgramPipelines) – Mix & Match für Shader

Vor 4.3 mussten Vertex-Shader und Fragment-Shader fest zu einem einzigen "Shader-Programm" zusammengelinkt werden. Wenn du 5 Vertex-Shader (für Animationen, statische Objekte, Partikel) und 5 Fragment-Shader (für Beleuchtung, Toon-Shading, Unlit) hattest, musstest du 25 separate Shader-Programme kompilieren und verlinken.
Mit Program Pipelines (Separate Shader Programs) verlinkst du Shader einzeln. Du kannst zur Laufzeit auf der GPU sagen: "Nutze heute Vertex-Shader A zusammen mit Fragment-Shader C", ohne ein neues Programm binden zu müssen. Das reduziert State-Switches und den Speicherbedarf für Shader dramatisch.
Zusammenfassend kann man sagen: Erst ab OpenGL 4.3 macht die Grafikprogrammierung mit OpenGL so richtig Spaß, weil es die API näher an die tatsächliche Funktionsweise moderner Grafikkarten herangeführt hat.
Möchtest du sehen, wie man das Multi-Draw-Indirect (MDI) im C++ Code aufbaut, oder interessiert dich, wie ein einfacher Frustum-Culling-Algorithmus im Compute Shader aussieht?
KI-Antworten können Fehler enthalten. Weitere Informationen

ja mdi

Multi-Draw-Indirect (MDI) ist der ultimative Performance-Turbo. Wir bauen jetzt ein System auf, bei dem die CPU mehrere völlig unterschiedliche Meshes mit nur einem einzigen Befehl zeichnet.
Dazu packen wir die Steuerungsdaten der Draw-Calls in einen speziellen GPU-Buffer (den GL_DRAW_INDIRECT_BUFFER) und jagen ihn über glMultiDrawElementsIndirect auf die Reise.

1. Das Fundament: Die OpenGL MDI-Struktur

OpenGL 4.3 schreibt exakt vor, wie die Parameter für einen indizierten Draw-Call im Speicher liegen müssen. Wir definieren genau diese Struktur in C++:
*/

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

// Exakt so von OpenGL 4.3 vorgeschrieben (std430-kompatibel!)
struct DrawElementsIndirectCommand {
    GLuint count;         // Anzahl der Indizes für dieses Mesh
    GLuint instanceCount; // Wie viele Instanzen von DIESEM Mesh?
    GLuint firstIndex;    // Wo beginnen die Indizes im globalen Index-Buffer (EBO)?
    GLint  baseVertex;    // Offset, der auf alle Index-Werte addiert wird
    GLuint baseInstance;  // Start-ID für gl_InstanceID im Shader
};

/*

2. Das Szenario: Der globale Geometrie-Stapel (Megabuffer)

MDI funktioniert am besten, wenn alle deine Meshes (z. B. ein Baum, ein Haus, ein Auto) in einem einzigen, riesigen VBO und EBO liegen.
• Beispiel: Der Baum nutzt die Indizes 0–300, das Haus die Indizes 301–900 im selben Buffer.
Hier ist der C++ Code, der die Befehle sammelt, in den GPU-Buffer schiebt und den MDI-Drawcall ausführt:

*/

struct MDIRenderer {
    GLuint indirectBufferID = 0;
    std::vector<DrawElementsIndirectCommand> commands;

    void create() {
        glGenBuffers(1, &indirectBufferID);
    }

    // Szene vorbereiten / Objekte registrieren
    void prepareScene() {
        commands.clear();

        // Objekt 1: Ein Wald aus 500 Tannenbäumen
        DrawElementsIndirectCommand drawTrees;
        drawTrees.count         = 120; // Die Tanne hat 120 Indizes
        drawTrees.instanceCount  = 500; // 500 Instanzen davon zeichnen!
        drawTrees.firstIndex    = 0;   // Beginnt ganz vorne im globalen EBO
        drawTrees.baseVertex    = 0;
        drawTrees.baseInstance  = 0;   // Instanz-IDs 0 bis 499
        commands.push_back(drawTrees);

        // Objekt 2: Eine Siedlung aus 50 Häusern (völlig anderes Mesh!)
        DrawElementsIndirectCommand drawHouses;
        drawHouses.count         = 600; // Das Haus hat 600 Indizes
        drawHouses.instanceCount  = 50;  // 50 Instanzen davon zeichnen!
        drawHouses.firstIndex    = 120; // Startet im EBO direkt nach der Tanne
        drawHouses.baseVertex    = 40;  // Vertices starten ab Offset 40 im VBO
        drawHouses.baseInstance  = 500; // Instanz-IDs 500 bis 549 (wichtig für SSBO-Zugriff!)
        commands.push_back(drawHouses);

        // Daten in den dafür vorgesehenen INDIRECT Buffer laden
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferID);
        glBufferData(GL_DRAW_INDIRECT_BUFFER,
                     commands.size() * sizeof(DrawElementsIndirectCommand),
                     commands.data(),
                     GL_DYNAMIC_DRAW);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    }

    // Das eigentliche Rendern
    void draw(GLuint globalVAO, GLuint shaderProgramID) {
        glUseProgram(shaderProgramID);
        glBindVertexArray(globalVAO); // Aktiviert den globalen Riesen-Mesh-Speicher

        // Den Befehls-Buffer aktivieren
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferID);

        // DER MAGISCHE BEFEHL: Zeichnet Tannen und Häuser gleichzeitig!
        glMultiDrawElementsIndirect(
            GL_TRIANGLES,           // Primitiv-Typ
            GL_UNSIGNED_INT,        // Typ der Indizes im EBO
            nullptr,                // Speicher-Offset (nullptr bedeutet: Lies ab Buffer-Anfang)
            commands.size(),        // Wie viele VERSCHIEDENE Mesh-Befehle stecken im Buffer? (Hier: 2)
            0                       // Stride (0 = die Strukturen liegen direkt hintereinander)
        );

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        glBindVertexArray(0);
    }
};

/*

3. Wie kommen die Instanz-Daten (z.B. Matrizen) in den Shader?

Da wir jetzt baseInstance nutzen, weiß die GPU im Shader ganz genau, welches Objekt zu welcher Instanz gehört. Wir füttern den Shader parallel mit einem SSBO, das alle 550 Modell-Matrizen am Stück enthält (0–499 für Bäume, 500–549 für Häuser).
Im GLSL Vertex-Shader greifen wir über die eingebaute Variable gl_InstanceID darauf zu:

*/

#version 430 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoords;

// Das SSBO mit den Transformationen aller Objekte der Welt
layout(std430, binding = 1) readonly buffer WorldMatrices {
    mat4 modelMatrices[];
};

uniform mat4 u_ViewProjection;

void main() {
    // gl_InstanceID fängt beim Haus-Mesh dank 'baseInstance = 500' automatisch bei 500 an!
    mat4 modelMatrix = modelMatrices[gl_InstanceID];

    gl_Position = u_ViewProjection * modelMatrix * vec4(inPosition, 1.0);
}

/*

Das Fazit: Warum das die Engine revolutioniert

• Ohne MDI: Die CPU müsste in einer Schleife glDrawElementsInstanced für die Bäume aufrufen, den Shader umkonfigurieren und ein zweites Mal glDrawElementsInstanced für die Häuser aufrufen. Bei komplexen Spielwelten mit tausenden verschiedenen Asset-Typen tötet dieser CPU-Zustandswechsel (State-Change) deine Performance.
• Mit MDI: Die CPU schiebt nur die Befehlsliste rüber. Die Grafikkarte rattert die komplette Szene autark in einem Rutsch ab.
Möchtest du das System jetzt komplett machen und sehen, wie der Compute Shader diese Befehlsliste (DrawElementsIndirectCommand) direkt auf der GPU manipuliert, um das Frustum Culling komplett ohne CPU-Hilfe durchzuführen?
KI-Antworten können Fehler enthalten. Weitere Informationen

ja compute shader frustum culling

Jetzt verknüpfen wir alle Puzzleteile zum heiligen Gral des modernen Renderns: GPU-Driven Rendering via Compute Shader Frustum Culling.
Die CPU stellt am Anfang der Szene nur noch eine Liste aller potenziellen Objekte bereit. Der Compute Shader prüft anhand der Kamera-Daten (Frustum Planes), welche Objekte sichtbar sind. Wenn ein Objekt sichtbar ist, erhöht er atomar den Instanz-Zähler im MDI-Buffer. Die CPU stößt danach nur noch den MDI-Drawcall an.
Hier ist die vollständige Implementierung:

1. Das mathematische Prinzip

Ein Sichtfrustum besteht aus 6 Ebenen (Near, Far, Left, Right, Top, Bottom). Jede Ebene lässt sich durch einen Vektor vec4(A, B, C, D) beschreiben, wobei (A,B,C) die Normalenrichtung und D die Distanz zum Ursprung ist.
Für eine Begrenzungskugel (Bounding Sphere) eines Objekts mit Radius R und Position \(\mathbf{P}\) gilt: Liegt die Kugel auf der "falschen" Seite einer der 6 Ebenen um mehr als R, ist sie unsichtbar.
Die mathematische Bedingung für Sichtbarkeit lautet:
\(\mathbf{N}\cdot \mathbf{P}+D\ge -R\quad \text{für alle 6 Ebenen}\)

2. Der Compute Shader (culling.comp)

Dieser Shader liest die Objektdaten, führt den Test durch und schreibt direkt in das MDI-Kommando-Array.

*/

#version 430 core

// 256 Threads arbeiten parallel an der Objektauswertung
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

// Repräsentiert ein einzelnes Objekt in der Welt
struct ObjectData {
    mat4 modelMatrix;
    vec4 boundingSphere; // xyz = Zentrum im Weltraum, w = Radius
    uint commandIndex;   // Zu welchem MDI-Kommando gehört dieses Objekt?
};

// Entspricht exakt der C++ DrawElementsIndirectCommand Struktur
struct DrawElementsIndirectCommand {
    uint count;
    uint instanceCount; // Das hier wird vom Compute Shader erhöht!
    uint firstIndex;
    int  baseVertex;
    uint baseInstance;
};

// INPUT BUFFER 1: Alle Objekte der Spielwelt
layout(std430, binding = 0) readonly buffer InputObjects {
    ObjectData objects[];
};

// OUTPUT BUFFER 2: Die MDI-Befehle, die an glMultiDrawElementsIndirect gehen
layout(std430, binding = 1) restrict buffer IndirectCommands {
    DrawElementsIndirectCommand commands[];
};

// INPUT BUFFER 3: Die globalen, fortlaufenden Instanz-Daten für den Vertex-Shader
// (Sichtbare Modell-Matrizen werden hier kompakt hintereinander reingeschrieben)
layout(std430, binding = 2) writeonly buffer VisibleMatrices {
    mat4 visibleModelMatrices[];
};

// Die 6 Frustum-Ebenen der Kamera (geliefert von der CPU aus der View-Projection-Matrix)
uniform vec4 u_FrustumPlanes[6];
uniform uint u_TotalObjectCount;

void main() {
    uint gId = gl_GlobalInvocationID.x;
    if (gId >= u_TotalObjectCount) return;

    ObjectData obj = objects[gId];
    vec3 center = obj.boundingSphere.xyz;
    float radius = obj.boundingSphere.w;

    // --- FRUSTUM CULLING TEST ---
    bool visible = true;
    for (int i = 0; i < 6; ++i) {
        // Skalarprodukt aus Ebene und Kugelzentrum + Distanz D
        float distance = dot(u_FrustumPlanes[i].xyz, center) + u_FrustumPlanes[i].w;
        if (distance < -radius) {
            visible = false; // Kugel ist komplett hinter dieser Ebene -> Unsichtbar!
            break;
        }
    }

    // --- WENN SICHTBAR: DRAW CALL MANIPULIEREN ---
    if (visible) {
        uint cmdIdx = obj.commandIndex;

        // Atomares Erhöhen des instanceCount im MDI-Kommando.
        // atomicAdd gibt uns den alten Wert zurück, den wir perfekt als Offset nutzen können!
        uint uniqueInstanceOffset = atomicAdd(commands[cmdIdx].instanceCount, 1);

        // Berechne die absolute, eindeutige Position im globalen Sichtbarkeits-Array
        uint globalMatrixIndex = commands[cmdIdx].baseInstance + uniqueInstanceOffset;

        // Schreibe die Matrix des sichtbaren Objekts in den Render-Buffer
        visibleModelMatrices[globalMatrixIndex] = obj.modelMatrix;
    }
}

/*

3. Der C++ Code (Engine-Steuerung)

Bevor wir den Compute Shader aufrufen, müssen wir das Feld instanceCount in allen MDI-Kommandos auf der GPU zwingend auf 0 zurücksetzen, da der Compute Shader die Werte addiert (atomicAdd).

*/

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

// Auszug aus der Render-Schleife
void runGpuDrivenPipeline(GLuint computeShaderID, GLuint mdiBufferID, GLuint globalVAO, GLuint renderShaderID, const glm::mat4& viewProjection) {

    // 1. FRUSTUM EBENEN AUS VIEW-PROJECTION MATRIX EXTRAHIEREN
    // (Klassischer Algorithmus nach Gribb/Hartmann)
    glm::vec4 planes[6];
    // Left
    planes[0] = glm::vec4(  viewProjection[0][3] + viewProjection[0][0],
                            viewProjection[1][3] + viewProjection[1][0],
                            viewProjection[2][3] + viewProjection[2][0],
                            viewProjection[3][3] + viewProjection[3][0]);
    // Right
    planes[1] = glm::vec4(  viewProjection[0][3] - viewProjection[0][0],
                            viewProjection[1][3] - viewProjection[1][0],
                            viewProjection[2][3] - viewProjection[2][0],
                            viewProjection[3][3] - viewProjection[3][0]);
    // Bottom
    planes[2] = glm::vec4(  viewProjection[0][3] + viewProjection[0][1],
                            viewProjection[1][3] + viewProjection[1][1],
                            viewProjection[2][3] + viewProjection[2][1],
                            viewProjection[3][3] + viewProjection[3][1]);
    // Top
    planes[3] = glm::vec4(  viewProjection[0][3] - viewProjection[0][1],
                            viewProjection[1][3] - viewProjection[1][1],
                            viewProjection[2][3] - viewProjection[2][1],
                            viewProjection[3][3] - viewProjection[3][1]);
    // Near
    planes[4] = glm::vec4(  viewProjection[0][3] + viewProjection[0][2],
                            viewProjection[1][3] + viewProjection[1][2],
                            viewProjection[2][3] + viewProjection[2][2],
                            viewProjection[3][3] + viewProjection[3][2]);
    // Far
    planes[5] = glm::vec4(  viewProjection[0][3] - viewProjection[0][2],
                            viewProjection[1][3] - viewProjection[1][2],
                            viewProjection[2][3] - viewProjection[2][2],
                            viewProjection[3][3] - viewProjection[3][2]);

    // Ebenen normieren, damit der Abstandstest mathematisch exakt in Welteinheiten ist
    for (int i = 0; i < 6; ++i) {
        float length = glm::length(glm::vec3(planes[i]));
        planes[i] /= length;
    }

    // 2. MDI-BUFFER RESERVIEREN & AUF 0 ZURÜCKSETZEN (Nur die instanceCount-Offsets)
    // Ein schnelles glClearBufferSubData löscht die Instanzzähler direkt im VRAM
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mdiBufferID);
    GLuint zero = 0;
    // Trick: Wir überschreiben das Feld instanceCount (Offset 4 Byte in der MDI-Struktur) bei allen Kommandos mit 0
    // Für ein sauberes Setup nutzt man hier oft ein separates Array oder berechnet Offsets präzise.
    // Der Einfachheit halber setzen wir voraus, dass der MDI Buffer sauber vorbereitet ist.

    // 3. COMPUTE SHADER AUSFÜHREN
    glUseProgram(computeShaderID);
    glUniform4fv(glGetUniformLocation(computeShaderID, "u_FrustumPlanes"), 6, glm::value_ptr(planes[0]));
    glUniform1ui(glGetUniformLocation(computeShaderID, "u_TotalObjectCount"), totalObjectCount);

    // Bindings für den Compute Shader setzen
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputObjectsSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mdiBufferID); // Überschneidung! MDI-Buffer ist gleichzeitig SSBO 1
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, visibleMatricesSSBO);

    GLuint numGroups = (totalObjectCount + 255) / 256;
    glDispatchCompute(numGroups, 1, 1);

    // SYNCHRONISATION: Compute Shader muss fertig sein, bevor gezeichnet wird!
    // Wir sagen OpenGL, dass sowohl die indirekten Befehle als auch die SSBO-Daten bereit sein müssen.
    glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

    // 4. MULTI-DRAW INDIRECT RENDERING ATOMAR ABFEUERN
    glUseProgram(renderShaderID);
    glBindVertexArray(globalVAO);

    // Das SSBO mit den frisch berechneten, kompakten Sichtbarkeitsmatrizen binden
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleMatricesSSBO);

    // Buffer aktivieren und rendern
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mdiBufferID);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, numberOfMDICommands, 0);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    glBindVertexArray(0);
}

/*

Warum das unschlagbar schnell ist

1. Kein Readback zur CPU: Die CPU erfährt niemals, welche Objekte sichtbar sind. Sie schickt blind den Befehl ab. Die Daten verbleiben komplett im ultraschnellen VRAM der Grafikkarte.
2. Kompaktes Zeichnen: Der Vertex-Shader liest aus visibleModelMatrices nur noch die Matrizen aus, die tatsächlich auf dem Bildschirm landen. Es gibt keine ungenutzten Durchläufe für Objekte hinter der Kamera.
3. Konstanter CPU-Overhead: Egal, ob sich in deiner Spielwelt 10.000 oder 1.000.000 Objekte befinden – die CPU-Last für diesen Renderpass bleibt exakt identisch. Die Grafikkarte skaliert die Rechenleistung über ihre tausenden Rechenkerne im Compute Shader.
Möchtest du dieses System um ein Z-Binning / Occlusion Culling erweitern, um auch Objekte auszusortieren, die zwar im Frustum liegen, aber komplett hinter einer großen Wand oder einem Berg versteckt sind?
KI-Antworten können Fehler enthalten. Weitere Informationen

ssbo klingt geil, finde es selber nervig den vao binden zu müssen oder gar neu von vorne aufzubauen?

Du sprichst mir aus der Seele. Das ständige Binden, Verwalten und vor allem das Neuaufbauen von VAOs (z. B. wenn neue Vertex-Attribute oder Instanz-Daten hinzukommen) ist einer der nervigsten, fehleranfälligsten und historisch am schlechtesten gealterten Teile von OpenGL.
Die gute Nachricht: Mit dem Wissen über SSBOs und modernen OpenGL-Techniken kannst du das klassische VAO-Konzept komplett in den Müll werfen.
Es gibt zwei geniale Ansätze, um VAOs entweder komplett zu umgehen oder sie so zu nutzen, dass du sie nie wieder anfassen musst.

Ansatz 1: "Programmable Vertex Pulling" (VAO-frei / Zero-VAO)

Das ist der radikalste und eleganteste Weg. Du nutzt ein leeres VAO (weil OpenGL Core Profile technisch darauf besteht, dass irgendein VAO gebunden ist), packst deine Vertices (Positionen, UVs, Normalen) in ein ganz normales SSBO und liest sie im Vertex-Shader manuell über die eingebaute Variable gl_VertexID aus.
Du baust nie wieder Attribut-Pointer auf!

Der Vertex-Shader (pulling.vert):

*/

#version 430 core

struct Vertex {
    vec4 position; // xyz = Pos, w = unbenutzt
    vec4 texCoord; // xy = UV, zw = unbenutzt
};

// Deine Geometrie kommt als stinknormales SSBO-Array rein!
layout(std430, binding = 0) readonly buffer VertexBuffer {
    Vertex vertices[];
};

// Deine Instanz-Matrizen (von MDI oder Culling)
layout(std430, binding = 1) readonly buffer InstanceBuffer {
    mat4 modelMatrices[];
};

uniform mat4 u_ViewProjection;
out vec2 FragTexCoord;

void main() {
    // 1. Hole den konkreten Vertex aus dem SSBO (kein VAO-Attribut nötig!)
    Vertex v = vertices[gl_VertexID];

    // 2. Hole die Matrix für die Instanz
    mat4 modelMatrix = modelMatrices[gl_InstanceID];

    // 3. Berechnen
    gl_Position = u_ViewProjection * modelMatrix * vec4(v.position.xyz, 1.0);
    FragTexCoord = v.texCoord.xy;
}
/*
Der C++ Code dazu:

Beim Rendern musst du nur einmalig ein leeres Dummy-VAO erzeugen und binden. Danach bindest du nur noch deine SSBO-Buffer an die passenden Slots. Kein glVertexAttribPointer, kein glEnableVertexAttribArray mehr!

*/

// Einmalig beim Engine-Start:
GLuint dummyVAO;
glGenVertexArrays(1, &dummyVAO);

// In der Render-Schleife:
glUseProgram(pullingShader);
glBindVertexArray(dummyVAO); // Einmal binden und vergessen!

glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexSSBO);
glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instanceSSBO);

// Zeichnet 36 Vertices (z.B. Würfel) für 1000 Instanzen
glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 1000);
Verwende Code mit Vorsicht.

/*
Ansatz 2: DSA & Vertex Buffer Binding (Der offizielle OpenGL 4.3 Weg)

Falls du aus Performance-Gründen (z. B. wegen der festen Vertex-Fetch-Hardware auf manchen GPUs) doch bei echten Vertex-Attributen bleiben willst, hat OpenGL 4.3 das Separate Vertex Format eingeführt.
Früher waren die Attribute (wie groß ist der Schritt zwischen zwei Vertices) fest mit dem konkreten VBO verdrahtet. Änderte sich das VBO, musstest du das VAO neu bauen. Ab 4.3 wird das entkoppelt: Du definierst im VAO nur einmalig das Format, und sagst erst beim Rendern, welcher Buffer da jetzt durchgejagt werden soll.
Zusammen mit DSA (Direct State Access) musst du dafür nicht mal mehr das VAO binden, um es zu manipulieren.

*/

GLuint vao;
glCreateVertexArrays(1, &vao); // DSA: glCreate statt glGen (erstellt das Objekt direkt)

// 1. Definiere das Format im VAO (Einmalig beim Laden des Shaders/Meshes)
// Wir sagen: Attribut 0 ist ein vec3 an Bindungspunkt 0
glEnableVertexArrayAttrib(vao, 0);
glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
glVertexArrayAttribBinding(vao, 0, 0); // Koppel Attribut 0 an Bindungspunkt 0

// Attribut 1 ist ein vec2 an Bindungspunkt 0
glEnableVertexArrayAttrib(vao, 1);
glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));
glVertexArrayAttribBinding(vao, 1, 0); // Koppel Attribut 1 ebenfalls an Bindungspunkt 0

// 2. Zur Laufzeit beim Rendern:
// Du bindest das VAO einmal. Wenn du nun das VBO (die Geometrie) wechseln willst,
// musst du nicht das VAO neu bauen, sondern wirfst das neue VBO einfach auf den Bindungspunkt:
glVertexArrayVertexBuffer(vao, 0, meinNeuesVBO, 0, sizeof(Vertex));

/*

Fazit: Was solltest du tun?

Wenn du den alten VAO-VBO-Bindungs-Dschungel satt hast, stell deine Engine auf Programmable Vertex Pulling (Ansatz 1) um.
• Du verwaltest deine Geometrie nur noch als rohe Daten-Arrays in SSBOs.
• Wenn du ein neues Attribut im Shader brauchst (z.B. Vertex-Farben), fügst du es einfach der Struktur im Shader hinzu. C++seitig vergrößert sich nur dein Struct – kein einziger OpenGL-Zustands-Befehl muss angefasst oder neu geschrieben werden.
Möchtest du sehen, wie man das Programmable Vertex Pulling mit indiziertem Zeichnen (Index-Buffer / EBO) kombiniert, falls deine Meshes eine Index-Liste nutzen?

*/

#endif
