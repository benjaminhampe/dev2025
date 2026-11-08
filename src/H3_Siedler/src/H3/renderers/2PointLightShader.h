
1. Anpassung im Vertex-Shader

Anstatt die Position des zweiten Lichts mühsam im Vertex-Shader in den View-Space zu transformieren, übergeben wir die Lichtpositionen am besten direkt im View-Space als Uniforms vom C++ Code aus. Das spart der GPU wertvolle Rechenzeit.
Der Vertex-Shader wird dadurch schlanker, da v_fragPosLight0 wegfällt. Da v_fragPos bereits im View-Space liegt, berechnen wir die Lichtrichtungen einfach direkt im Fragment-Shader.
glsl
const std::string vs = R"(
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_tex;
layout(location = 3) in mat4 a_instanceMat;

uniform mat4 u_projMat;
uniform mat4 u_viewMat;

out vec2 v_tex;
out vec3 v_fragPos;
out vec3 v_fragNormal;

void main()
{
    v_tex = a_tex;

    vec4 fragPos = u_viewMat * a_instanceMat * vec4(a_pos, 1.0);
    gl_Position = u_projMat * fragPos;
    v_fragPos = vec3(fragPos);

    // Normalenmatrix (Hier kannst du u.U. auf transpose(inverse()) verzichten, siehe vorigen Tipp)
    mat3 normalMat = mat3(transpose(inverse(u_viewMat * a_instanceMat)));
    v_fragNormal = normalMat * a_normal; 
}
)";
Verwende Code mit Vorsicht.

2. Der neue Fragment-Shader (mit 2 Lichtquellen)

Wir fügen eine zweite Lichtposition und -farbe hinzu (u_lightPos1 und u_lightColor1). Im main berechnen wir Diffuse und Specular einfach für beide Lichter getrennt und addieren sie zusammen.
glsl
const std::string fs = R"(
#version 330 core

out vec4 fragColor;

in vec2 v_tex;
in vec3 v_fragPos;
in vec3 v_fragNormal;

uniform sampler2D u_diffuseMap;

// Licht 0 (Dein Front-/Kameralicht)
uniform vec3 u_lightPos0; // Bereits im View-Space übergeben!
uniform vec3 u_lightColor0;

// Licht 1 (Das neue Top-Licht von oben)
uniform vec3 u_lightPos1; // Bereits im View-Space übergeben!
uniform vec3 u_lightColor1;

void main()
{
    vec4 objectColor = texture(u_diffuseMap, v_tex);

    vec3 normal = normalize(v_fragNormal);
    vec3 viewDir = normalize(-v_fragPos); 

    // Globaler Ambient-Anteil (z.B. basierend auf Licht 0)
    float ambientStrength = 0.15;
    vec3 ambientColor = ambientStrength * u_lightColor0;

    // --- BERECHNUNG LICHT 0 (Front) ---
    vec3 lightDir0 = normalize(u_lightPos0 - v_fragPos); 
    float diff0 = max(dot(normal, lightDir0), 0.0);
    vec3 diffuse0 = diff0 * u_lightColor0;

    float specularStrength0 = 0.25;
    vec3 reflectDir0 = reflect(-lightDir0, normal);
    float spec0 = pow(max(dot(viewDir, reflectDir0), 0.0), 32);
    vec3 specular0 = specularStrength0 * spec0 * u_lightColor0;

    // --- BERECHNUNG LICHT 1 (Top) ---
    vec3 lightDir1 = normalize(u_lightPos1 - v_fragPos); 
    float diff1 = max(dot(normal, lightDir1), 0.0);
    vec3 diffuse1 = diff1 * u_lightColor1;

    float specularStrength1 = 0.15; // Etwas schwächeres Glanzlicht von oben
    vec3 reflectDir1 = reflect(-lightDir1, normal);
    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), 32);
    vec3 specular1 = specularStrength1 * spec1 * u_lightColor1;

    // --- FINALE KOMBINATION ---
    // Alle diffusen und ambienten Anteile beleuchten die Textur
    vec3 totalDiffuse = ambientColor + diffuse0 + diffuse1;
    // Alle Glanzlichter werden rein addiert
    vec3 totalSpecular = specular0 + specular1;

    vec3 finalLighting = totalDiffuse * objectColor.rgb + totalSpecular;

    fragColor = vec4(finalLighting, objectColor.a);
}
)";
Verwende Code mit Vorsicht.

3. Wichtig: Übergabe der Uniforms in C++

Da wir die Transformationen aus dem Vertex-Shader entfernt haben, musst du die Welt-Positionen der Lichter nun in C++ in den View-Space rechnen, bevor du sie an den Shader schickst.
Das machst du für jedes Frame mit deiner aktuellen Kamera-Matrix (viewMatrix):
cpp
// Licht 0: Direkt bei der Kamera (z.B. leicht versetzt nach hinten/oben)
glm::vec3 worldLightPos0 = cameraPosition + glm::vec3(0.0f, 2.0f, 5.0f);
// In View-Space transformieren (w = 1.0 für Positionen!)
glm::vec3 viewLightPos0 = glm::vec3(viewMatrix * glm::vec4(worldLightPos0, 1.0f));

// Licht 1: Fixiert weit oben in der Spielwelt (leuchtet immer von oben herab)
glm::vec3 worldLightPos1 = glm::vec3(0.0f, 50.0f, 0.0f); 
glm::vec3 viewLightPos1 = glm::vec3(viewMatrix * glm::vec4(worldLightPos1, 1.0f));

// Jetzt an den Shader übergeben:
glUniform3fv(glGetUniformLocation(shaderProgram, "u_lightPos0"), 1, &viewLightPos0[0]);
glUniform3fv(glGetUniformLocation(shaderProgram, "u_lightPos1"), 1, &viewLightPos1[0]);
Verwende Code mit Vorsicht.

Ein alternativer Design-Tipp: Directional Light

Wenn das Licht von oben eine Art "Sonne" oder gleichmäßiges Deckenlicht sein soll, bei dem es egal ist, wie tief du gehst, kannst du Licht 1 auch als Directional Light (gerichtetes Licht) implementieren. Dabei zieht man keine Positionen voneinander ab, sondern übergibt einfach eine feste Richtung (z. B. immer gerade nach unten glm::vec3(0.0f, -1.0f, 0.0f)).
Möchtest du das zweite Licht als punktförmige Lampe (Position wichtig) oder lieber als regnerisches/sonnenartiges Licht von oben (Richtung überall gleich) verwenden? Ich kann dir zeigen, wie du die Berechnungen für ein Richtungslicht im Code anpasst.