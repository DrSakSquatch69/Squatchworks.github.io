using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DayNightCycle : MonoBehaviour
{
    [Header("Light Settings")]
    public Light sun; // The main sun light in the scene
    public float dayLength = 120f; // Time in seconds for a full day cycle
    private float timeOfDay = 0f;

    [Header("Color Settings")]
    public Color sunriseColor = new Color(1f, 0.6f, 0.2f); // Sunrise color
    public Color noonColor = new Color(1f, 1f, 0.8f); // Noon color
    public Color sunsetColor = new Color(1f, 0.4f, 0.1f); // Sunset color
    public Color nightColor = new Color(0.1f, 0.1f, 0.4f); // Night color

    [Header("Skybox")]
    public Material skyboxMaterial;
    public Color dayFogColor;
    public Color nightFogColor;

    private void Update()
    {
        // Advance time based on dayLength
        timeOfDay += Time.deltaTime / dayLength;
        if (timeOfDay > 1f) timeOfDay = 0f; // Reset at end of day cycle

        UpdateSunPosition();
        UpdateLighting();
        UpdateSkybox();
    }
    //test
    private void UpdateSunPosition()
    {
        // Set rotation to mimic the sun’s movement across the sky
        float angle = timeOfDay * 360f - 90f; // Start at sunrise (east)
        sun.transform.rotation = Quaternion.Euler(angle, 170f, 0f); // Adjust angle for sunrise/sunset direction
    }

    private void UpdateLighting()
    {
        // Smoothly transition the sun's color based on time of day
        if (timeOfDay < 0.25f) // Sunrise
            sun.color = Color.Lerp(nightColor, sunriseColor, timeOfDay * 4f);
        else if (timeOfDay < 0.5f) // Morning to Noon
            sun.color = Color.Lerp(sunriseColor, noonColor, (timeOfDay - 0.25f) * 4f);
        else if (timeOfDay < 0.75f) // Noon to Sunset
            sun.color = Color.Lerp(noonColor, sunsetColor, (timeOfDay - 0.5f) * 4f);
        else // Sunset to Night
            sun.color = Color.Lerp(sunsetColor, nightColor, (timeOfDay - 0.75f) * 4f);

        // Adjust light intensity based on the time of day
        sun.intensity = Mathf.Lerp(0.1f, 1f, Mathf.Sin(timeOfDay * Mathf.PI));
    }

    private void UpdateSkybox()
    {
        // Adjust fog and skybox color to reflect day/night changes
        RenderSettings.fogColor = Color.Lerp(nightFogColor, dayFogColor, Mathf.Sin(timeOfDay * Mathf.PI));

        if (skyboxMaterial)
        {
            // Modify the skybox color based on time of day
            skyboxMaterial.SetColor("_Tint", Color.Lerp(nightColor, noonColor, Mathf.Sin(timeOfDay * Mathf.PI)));
        }
    }
}
