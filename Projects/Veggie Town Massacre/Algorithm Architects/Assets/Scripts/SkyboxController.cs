using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SkyboxController : MonoBehaviour
{
    public Material skyboxMaterial;
    public float dayLength = 120f; // Time for a full day cycle

    private float timeOfDay = 0f;

    private void Update()
    {
        // Update time of day
        timeOfDay += Time.deltaTime / dayLength;
        if (timeOfDay > 1f) timeOfDay = 0f; // Reset after a full day cycle

        // Adjust BlendFactor in the skybox material for day-night effect
        float blendFactor = 1 - Mathf.Sin(timeOfDay * Mathf.PI);
        skyboxMaterial.SetFloat("_BlendFactor", blendFactor);
    }
}
