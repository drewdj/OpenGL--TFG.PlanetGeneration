#version 430

  #define saturate(a) clamp( a, 0.0, 1.0 )

uniform vec4 lightPos;
uniform vec4 camPos;
uniform bool atmosfera;

uniform float planetRadius;
uniform float atmosphereRadius;
uniform vec3 rayleighScattering;
uniform float mieScattering;
uniform vec2 hesightScale;
uniform float refraction;

  #define PI 3.141592
  #define PRIMARY_STEP_COUNT 16
  #define LIGHT_STEP_COUNT 8

in vec4 fpos;
in vec4 fnorm;

out vec4 gli_FragColor;




bool _RayIntersectsSphere(
      vec3 rayStart, vec3 rayDir, vec3 sphereCenter, float sphereRadius, out float t0, out float t1) {
    vec3 oc = rayStart - sphereCenter;
    float a = dot(rayDir, rayDir);
    float b = 2.0 * dot(oc, rayDir);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float d =  b * b - 4.0 * a * c;

    // Also skip single point of contact
    if (d <= 0.0) {
      return false;
    }

    float r0 = (-b - sqrt(d)) / (2.0 * a);
    float r1 = (-b + sqrt(d)) / (2.0 * a);

    t0 = min(r0, r1);
    t1 = max(r0, r1);

    return (t1 >= 0.0);
  }

  vec3 _SampleLightRay(
      vec3 origin, vec3 sunDir, float planetScale, float planetRadius, float totalRadius,
      float rayleighScale, float mieScale, float absorptionHeightMax, float absorptionFalloff) {

    float t0, t1;
    _RayIntersectsSphere(origin, sunDir, fpos.xyz, totalRadius, t0, t1);

    float actualLightStepSize = (t1 - t0) / float(LIGHT_STEP_COUNT);
    float virtualLightStepSize = actualLightStepSize * planetScale;
    float lightStepPosition = 0.0;

    vec3 opticalDepthLight = vec3(0.0);

    for (int j = 0; j < LIGHT_STEP_COUNT; j++) {
      vec3 currentLightSamplePosition = origin + sunDir * (lightStepPosition + actualLightStepSize * 0.5);

      // Calculate the optical depths and accumulate
      float currentHeight = length(currentLightSamplePosition) - planetRadius;
      float currentOpticalDepthRayleigh = exp(-currentHeight / rayleighScale) * virtualLightStepSize;
      float currentOpticalDepthMie = exp(-currentHeight / mieScale) * virtualLightStepSize;
      float currentOpticalDepthOzone = (1.0 / cosh((absorptionHeightMax - currentHeight) / absorptionFalloff));
      currentOpticalDepthOzone *= currentOpticalDepthRayleigh * virtualLightStepSize;

      opticalDepthLight += vec3(
          currentOpticalDepthRayleigh,
          currentOpticalDepthMie,
          currentOpticalDepthOzone);

      lightStepPosition += actualLightStepSize;
    }

    return opticalDepthLight;
  }

    float _SoftLight(float a, float b) {
    return (b < 0.5 ?
        (2.0 * a * b + a * a * (1.0 - 2.0 * b)) :
        (2.0 * a * (1.0 - b) + sqrt(a) * (2.0 * b - 1.0))
    );
  }

  vec3 _SoftLight(vec3 a, vec3 b) {
    return vec3(
        _SoftLight(a.x, b.x),
        _SoftLight(a.y, b.y),
        _SoftLight(a.z, b.z)
    );
  }

   void _ComputeScattering(
      vec3 worldSpacePos, vec3 rayDirection, vec3 rayOrigin, vec3 sunDir,
      out vec3 scatteringColour, out vec3 scatteringOpacity) {

    float t0, t1;
    float distToPoint = length(worldSpacePos - rayOrigin);
    
    // This is a hack since the world mesh has seams that we haven't fixed yet.
    if (_RayIntersectsSphere(
        rayOrigin, rayDirection, fpos.xyz, planetRadius, t0, t1)) {
      if (distToPoint > t0) {
        worldSpacePos = rayOrigin + t0 * rayDirection;
      }
    }

    vec3 betaRayleigh = vec3(5.5e-6, 13.0e-6, 22.4e-6);
    float betaMie = 21e-6;
    vec3 betaAbsorption = vec3(2.04e-5, 4.97e-5, 1.95e-6);
    float g = 0.76;
    float sunIntensity = 20.0;

    float planetRadius = planetRadius;
    float atmosphereRadius = atmosphereRadius - planetRadius;
    float totalRadius = planetRadius + atmosphereRadius;

    float referencePlanetRadius = 6371000.0;
    float referenceAtmosphereRadius = 100000.0;
    float referenceTotalRadius = referencePlanetRadius + referenceAtmosphereRadius;
    float referenceRatio = referencePlanetRadius / referenceAtmosphereRadius;

    float scaleRatio = planetRadius / atmosphereRadius;
    float planetScale = referencePlanetRadius / planetRadius;
    float atmosphereScale = scaleRatio / referenceRatio;
    float maxDist = distance(worldSpacePos, rayOrigin);

    float rayleighScale = 8500.0 / (planetScale * atmosphereScale);
    float mieScale = 1200.0 / (planetScale * atmosphereScale);
    float absorptionHeightMax = 32000.0 * (planetScale * atmosphereScale);
    float absorptionFalloff = 3000.0 / (planetScale * atmosphereScale);;

    float mu = dot(rayDirection, sunDir);
    float mumu = mu * mu;
    float gg = g * g;
    float phaseRayleigh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float phaseMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

    // Early out if ray doesn't intersect atmosphere.
    if (!_RayIntersectsSphere(rayOrigin, rayDirection, fpos.xyz, totalRadius, t0, t1)) {
      scatteringOpacity = vec3(1.0);
      return;
    }

    // Clip the ray between the camera and potentially the planet surface.
    t0 = max(0.0, t0);
    t1 = min(maxDist, t1);

    float actualPrimaryStepSize = (t1 - t0) / float(PRIMARY_STEP_COUNT);
    float virtualPrimaryStepSize = actualPrimaryStepSize * planetScale;
    float primaryStepPosition = 0.0;

    vec3 accumulatedRayleigh = vec3(0.0);
    vec3 accumulatedMie = vec3(0.0);
    vec3 opticalDepth = vec3(0.0);

    // Take N steps along primary ray
    for (int i = 0; i < PRIMARY_STEP_COUNT; i++) {
      vec3 currentPrimarySamplePosition = rayOrigin + rayDirection * (
          primaryStepPosition + actualPrimaryStepSize * 0.5);

      float currentHeight = max(0.0, length(currentPrimarySamplePosition) - planetRadius);

      float currentOpticalDepthRayleigh = exp(-currentHeight / rayleighScale) * virtualPrimaryStepSize;
      float currentOpticalDepthMie = exp(-currentHeight / mieScale) * virtualPrimaryStepSize;

      // Taken from https://www.shadertoy.com/view/wlBXWK
      float currentOpticalDepthOzone = (1.0 / cosh((absorptionHeightMax - currentHeight) / absorptionFalloff));
      currentOpticalDepthOzone *= currentOpticalDepthRayleigh * virtualPrimaryStepSize;

      opticalDepth += vec3(currentOpticalDepthRayleigh, currentOpticalDepthMie, currentOpticalDepthOzone);

      // Sample light ray and accumulate optical depth.
      vec3 opticalDepthLight = _SampleLightRay(
          currentPrimarySamplePosition, sunDir,
          planetScale, planetRadius, totalRadius,
          rayleighScale, mieScale, absorptionHeightMax, absorptionFalloff);

      vec3 r = (
          betaRayleigh * (opticalDepth.x + opticalDepthLight.x) +
          betaMie * (opticalDepth.y + opticalDepthLight.y) + 
          betaAbsorption * (opticalDepth.z + opticalDepthLight.z));
      vec3 attn = exp(-r);

      accumulatedRayleigh += currentOpticalDepthRayleigh * attn;
      accumulatedMie += currentOpticalDepthMie * attn;

      primaryStepPosition += actualPrimaryStepSize;
    }

    scatteringColour = sunIntensity * (phaseRayleigh * betaRayleigh * accumulatedRayleigh + phaseMie * betaMie * accumulatedMie);
    scatteringOpacity = exp(
        -(betaMie * opticalDepth.y + betaRayleigh * opticalDepth.x + betaAbsorption * opticalDepth.z));
  }

vec3 _ApplyGroundFog(
      in vec3 rgb,
      float distToPoint,
      float height,
      in vec3 worldSpacePos,
      in vec3 rayOrigin,
      in vec3 rayDir,
      in vec3 sunDir)
  {
    vec3 up = normalize(rayOrigin);

    float skyAmt = dot(up, rayDir) * 0.25 + 0.75;
    skyAmt = saturate(skyAmt);
    skyAmt *= skyAmt;

    vec3 DARK_BLUE = vec3(0.1, 0.2, 0.3);
    vec3 LIGHT_BLUE = vec3(0.5, 0.6, 0.7);
    vec3 DARK_ORANGE = vec3(0.7, 0.4, 0.05);
    vec3 BLUE = vec3(0.5, 0.6, 0.7);
    vec3 YELLOW = vec3(1.0, 0.9, 0.7);

    vec3 fogCol = mix(DARK_BLUE, LIGHT_BLUE, skyAmt);
    float sunAmt = max(dot(rayDir, sunDir), 0.0);
    fogCol = mix(fogCol, YELLOW, pow(sunAmt, 16.0));

    float be = 0.0025;
    float fogAmt = (1.0 - exp(-distToPoint * be));

    // Sun
    sunAmt = 0.5 * saturate(pow(sunAmt, 256.0));

    return mix(rgb, fogCol, fogAmt) + sunAmt * YELLOW;
  }

vec3 _ApplySpaceFog(
      in vec3 rgb,
      in float distToPoint,
      in float height,
      in vec3 worldSpacePos,
      in vec3 rayOrigin,
      in vec3 rayDir,
      in vec3 sunDir)
  {
    float atmosphereThickness = (atmosphereRadius - planetRadius);

    float t0 = -1.0;
    float t1 = -1.0;

    // This is a hack since the world mesh has seams that we haven't fixed yet.
    if (_RayIntersectsSphere(
        rayOrigin, rayDir, fpos.xyz, planetRadius, t0, t1)) {
      if (distToPoint > t0) {
        distToPoint = t0;
        worldSpacePos = rayOrigin + t0 * rayDir;
      }
    }

    if (!_RayIntersectsSphere(
        rayOrigin, rayDir, fpos.xyz, planetRadius + atmosphereThickness * 5.0, t0, t1)) {
      return rgb * 0.5;
    }

    // Figure out a better way to do this
    float silhouette = saturate((distToPoint - 10000.0) / 10000.0);

    // Glow around planet
    float scaledDistanceToSurface = 0.0;

    // Calculate the closest point between ray direction and planet. Use a point in front of the
    // camera to force differences as you get closer to planet.
    vec3 fakeOrigin = rayOrigin + rayDir * atmosphereThickness;
    float t = max(0.0, dot(rayDir, fpos.xyz - fakeOrigin) / dot(rayDir, rayDir));
    vec3 pb = fakeOrigin + t * rayDir;

    scaledDistanceToSurface = saturate((distance(pb, fpos.xyz) - planetRadius) / atmosphereThickness);
    scaledDistanceToSurface = smoothstep(0.0, 1.0, 1.0 - scaledDistanceToSurface);
    //scaledDistanceToSurface = smoothstep(0.0, 1.0, scaledDistanceToSurface);

    float scatteringFactor = scaledDistanceToSurface * silhouette;

    // Fog on surface
    t0 = max(0.0, t0);
    t1 = min(distToPoint, t1);

    vec3 intersectionPoint = rayOrigin + t1 * rayDir;
    vec3 normalAtIntersection = normalize(intersectionPoint);

    float distFactor = exp(-distToPoint * 0.0005 / (atmosphereThickness));
    float fresnel = 1.0 - saturate(dot(-rayDir, normalAtIntersection));
    fresnel = smoothstep(0.0, 1.0, fresnel);

    float extinctionFactor = saturate(fresnel * distFactor) * (1.0 - silhouette);

    // Front/Back Lighting
    vec3 BLUE = vec3(0.5, 0.6, 0.75);
    vec3 YELLOW = vec3(1.0, 0.9, 0.7);
    vec3 RED = vec3(0.035, 0.0, 0.0);

    float NdotL = dot(normalAtIntersection, sunDir);
    float wrap = 0.5;
    float NdotL_wrap = max(0.0, (NdotL + wrap) / (1.0 + wrap));
    float RdotS = max(0.0, dot(rayDir, sunDir));
    float sunAmount = RdotS;

    vec3 backLightingColour = YELLOW * 0.1;
    vec3 frontLightingColour = mix(BLUE, YELLOW, pow(sunAmount, 32.0));

    vec3 fogColour = mix(backLightingColour, frontLightingColour, NdotL_wrap);

    extinctionFactor *= NdotL_wrap;

    // Sun
    float specular = pow((RdotS + 0.5) / (1.0 + 0.5), 64.0);

    fresnel = 1.0 - saturate(dot(-rayDir, normalAtIntersection));
    fresnel *= fresnel;

    float sunFactor = (length(pb) - planetRadius) / (atmosphereThickness * 5.0);
    sunFactor = (1.0 - saturate(sunFactor));
    sunFactor *= sunFactor;
    sunFactor *= sunFactor;
    sunFactor *= specular * fresnel;

    vec3 baseColour = mix(rgb, fogColour, extinctionFactor);
    vec3 litColour = baseColour + _SoftLight(fogColour * scatteringFactor + YELLOW * sunFactor, baseColour);
    vec3 blendedColour = mix(baseColour, fogColour, scatteringFactor);
    blendedColour += blendedColour + _SoftLight(YELLOW * sunFactor, blendedColour);
    return mix(litColour, blendedColour, scaledDistanceToSurface * 0.25);
  }



vec3 _ApplyFog(
    in vec3 rgb,
    in float distToPoint,
    in float height,
    in vec3 worldSpacePos,
    in vec3 rayOrigin,
    in vec3 rayDir,
    in vec3 sunDir)
  {
    float distToPlanet = max(0.0, length(rayOrigin) - planetRadius);
    float atmosphereThickness = (atmosphereRadius - planetRadius);

    vec3 groundCol = _ApplyGroundFog(
      rgb, distToPoint, height, worldSpacePos, rayOrigin, rayDir, sunDir);
    vec3 spaceCol = _ApplySpaceFog(
      rgb, distToPoint, height, worldSpacePos, rayOrigin, rayDir, sunDir);

    float blendFactor = saturate(distToPlanet / (atmosphereThickness * 0.5));

    blendFactor = smoothstep(0.0, 1.0, blendFactor);
    blendFactor = smoothstep(0.0, 1.0, blendFactor);

    return mix(groundCol, spaceCol, blendFactor);
  }


void main() {

	float dist = distance(fpos, camPos);
    float height = max(0.0, length(camPos) - planetRadius);

    vec3 cameraDirection = normalize(fpos.xyz - camPos.xyz);
    vec3 rayDir = normalize(fpos.xyz - camPos.xyz);
    vec3 sunDir = normalize(lightPos.xyz - fpos.xyz);


    vec3 diffuse = _ApplyFog(rayleighScattering, dist, height, fpos.xyz,camPos.xyz , rayDir , sunDir);






  /* vec3 scatteringColour = vec3(0.0);
    vec3 scatteringOpacity = vec3(1.0, 1.0, 1.0);
    _ComputeScattering(
        fpos.xyz, cameraDirection, camPos.xyz,
        sunDir, scatteringColour, scatteringOpacity
    );

    // diffuse = pow(diffuse, vec3(1.0 / 2.0));
    diffuse = diffuse * scatteringOpacity + scatteringColour;
    diffuse = pow(diffuse, vec3(1.0 / 2.0));
    // diffuse = ACESFilmicToneMapping(diffuse); */

    if(atmosfera){
        gli_FragColor = vec4(diffuse, 0.2);
    }else{
        gli_FragColor = vec4(diffuse, 0);
    }

    
}

