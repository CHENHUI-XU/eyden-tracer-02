#pragma once

#include "ShaderFlat.h"

class CScene;

class CShaderPhong : public CShaderFlat
{
public:
	/**
	 * @brief Constructor
	 * @param scene Reference to the scene
	 * @param color The color of the object
	 * @param ka The ambient coefficient
	 * @param kd The diffuse reflection coefficients
	 * @param ks The specular refelection coefficients
	 * @param ke The shininess exponent
	 */
	CShaderPhong(CScene& scene, Vec3f color, float ka, float kd, float ks, float ke)
		: CShaderFlat(color)
		, m_scene(scene)
		, m_ka(ka)
		, m_kd(kd)
		, m_ks(ks)
		, m_ke(ke)
	{}
	virtual ~CShaderPhong(void) = default;

	virtual Vec3f Shade(const Ray& ray) const override
	{
		// --- PUT YOUR CODE HERE ---
		Vec3f a_color = m_ka * m_color;
		
		Vec3f lightSourceDiffuseSum = 0;
		Ray lightRay;
		for (int i = 0; i<m_scene.m_vpLights.size(); i++) {
			lightRay.org = ray.org + ray.dir * ray.t;
			std::optional<Vec3f> lightRadiance = m_scene.m_vpLights[i]->Illuminate(lightRay);
			lightRay.t = std::numeric_limits<float>::infinity();
			if (!m_scene.Occluded(lightRay)) {
				if (lightRadiance) {
					float cosTheta = max(0.0f,lightRay.dir.dot(ray.hit->GetNormal(ray)));
					lightSourceDiffuseSum += *lightRadiance * cosTheta;
				}
			}
		}
		Vec3f d_color = m_kd * lightSourceDiffuseSum.mul(m_color);

		Vec3f lightSourceSpecularSum = 0;
		Ray incidentRay;
		for (int i = 0; i<m_scene.m_vpLights.size(); i++) {
			incidentRay.org = ray.org + ray.dir * ray.t;
			std::optional<Vec3f> lightRadiance = m_scene.m_vpLights[i]->Illuminate(incidentRay);
			incidentRay.t = std::numeric_limits<float>::infinity();
			if (!m_scene.Occluded(lightRay)) {
				if (lightRadiance) {
					Vec3f reflectedDir = incidentRay.dir - 2 * (incidentRay.dir.dot(ray.hit->GetNormal(ray))) * ray.hit->GetNormal(ray);
					float cosTheta = max(0.0f, ray.dir.dot(reflectedDir));
					lightSourceSpecularSum += *lightRadiance * pow(cosTheta, m_ke);
				}
			}
		}
		Vec3f s_color = m_ks * RGB(1, 1, 1).mul(lightSourceSpecularSum);
		
		return d_color + s_color + a_color;
	}

	
private:
	CScene& m_scene;
	float 	m_ka;    ///< ambient coefficient
	float 	m_kd;    ///< diffuse reflection coefficients
	float 	m_ks;    ///< specular refelection coefficients
	float 	m_ke;    ///< shininess exponent
};
