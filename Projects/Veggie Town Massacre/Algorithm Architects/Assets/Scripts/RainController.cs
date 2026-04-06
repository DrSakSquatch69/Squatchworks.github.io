using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RainController : MonoBehaviour
{
    public ParticleSystem rainParticleSystem; // Drag the Particle System here in the Inspector
    public GameObject mudPrefab; // Drag the Mud Prefab here in the Inspector
    public float minRainInterval = 10f; // Minimum time between rains
    public float maxRainInterval = 30f; // Maximum time between rains
    public float rainDuration = 15f; // Duration of each rain event
    public float minRainIntensity = 50f; // Minimum emission rate
    public float maxRainIntensity = 1000f; // Maximum emission rate

    private ParticleSystem.EmissionModule emissionModule;
    private bool isRaining = false;

    void Start()
    {
        if (rainParticleSystem == null)
        {
            //Debug.LogError("Rain Particle System is not assigned!");
            return;
        }

        // Initialize mud prefab state
        if (mudPrefab != null)
        {
            //Debug.Log("Mud prefab found and initialized.");
            mudPrefab.SetActive(false);
        }
        else
        {
            //Debug.LogError("Mud prefab is not assigned!");
        }

        emissionModule = rainParticleSystem.emission;
        emissionModule.enabled = true;
        rainParticleSystem.Stop();

        StartCoroutine(RainCycle());
    }

    IEnumerator RainCycle()
    {
        while (true)
        {
            float waitTime = Random.Range(minRainInterval, maxRainInterval);
            yield return new WaitForSeconds(waitTime);

            float rainIntensity = Random.Range(minRainIntensity, maxRainIntensity);
            StartCoroutine(FadeRainIntensity(rainIntensity, 2f));

            if (!isRaining)
            {
                rainParticleSystem.Play();
                isRaining = true;
                //Debug.Log("Rain started. Enabling mud effect.");
                EnableMudEffect();
            }

            yield return new WaitForSeconds(rainDuration);

            StartCoroutine(FadeRainIntensity(0f, 2f));
            yield return new WaitForSeconds(2f);

            // Stop rain and disable mud effect
            if (isRaining)
            {
                rainParticleSystem.Stop();
                isRaining = false;
                DisableMudEffect();
            }
        }
    }

    IEnumerator FadeRainIntensity(float targetIntensity, float duration)
    {
        float startIntensity = emissionModule.rateOverTime.constant;
        float elapsedTime = 0f;

        while (elapsedTime < duration)
        {
            float currentIntensity = Mathf.Lerp(startIntensity, targetIntensity, elapsedTime / duration);
            emissionModule.rateOverTime = currentIntensity;
            elapsedTime += Time.deltaTime;
            yield return null;
        }

        emissionModule.rateOverTime = targetIntensity;
    }

    private void EnableMudEffect()
    {
        if (mudPrefab != null)
        {
            if (!mudPrefab.activeSelf)
            {
                mudPrefab.SetActive(true);
                //Debug.Log("Mud prefab activated.");
            }
            else
            {
                //Debug.LogWarning("Mud prefab was already active.");
            }
        }
    }

    private void DisableMudEffect()
    {
        if (mudPrefab != null && mudPrefab.activeSelf)
        {
            mudPrefab.SetActive(false);
            //Debug.Log("Mud prefab deactivated.");
        }
        else
        {
            //Debug.Log("Mud prefab is already inactive or not assigned.");
        }
    }
}
