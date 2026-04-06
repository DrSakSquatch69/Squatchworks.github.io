using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.PostProcessing;

public class ToxicGasTrigger : MonoBehaviour
{
    public ParticleSystem gasEffect;
    public float poisonDamage = 1f;
    public float poisonInterval = 1f;
    public PostProcessVolume postProcessVolume;

    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            gasEffect.Play();
            EnableBlur();
            StartCoroutine(ApplyPoisonDamage(other.GetComponent<PlayerController>()));
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            gasEffect.Stop();
            DisableBlur();
            StartCoroutine(ApplyPoisonDamage(other.GetComponent<PlayerController>()));
        }
    }

    IEnumerator ApplyPoisonDamage(PlayerController playerController)
    {
        while (true)
        {
            yield return new WaitForSeconds(poisonInterval);
            playerController.takeDamage((int)poisonDamage, Vector3.zero, damageType.stationary);
        }
    }

    void EnableBlur()
    {
        // Enable blur post-processing effect
        postProcessVolume.weight = 1; // Adjust the weight value as needed
    }

    void DisableBlur()
    {
        // Disable blur post-processing effect
        postProcessVolume.weight = 0;
    }
}