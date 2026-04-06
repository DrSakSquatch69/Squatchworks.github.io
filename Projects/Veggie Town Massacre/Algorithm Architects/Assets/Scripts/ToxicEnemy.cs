using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.PostProcessing;


public class ToxicEnemy : MonoBehaviour 
{
    [SerializeField] private Renderer enemyRenderer;
    [SerializeField] private Vector2 materialScale = new Vector2(1f, 1f);
    public float gasRange = 5f;
    public float attachRange = 1f;
    public float poisonDamage = 1f;
    public float poisonInterval = 1f;
    public float attachDuration = 5f;
    private bool isAttached = false;
    private Transform player;

    // Particle system and post-processing fields
    public ParticleSystem gasEffect;
    public Camera playerCamera;
    public PostProcessVolume postProcessVolume;  // Used for post-processing effects
    private DepthOfField depthOfFieldEffect;


    void Start()
    {
        player = GameObject.FindGameObjectWithTag("Player").transform;
        // Check if the volume has Depth of Field and cache it
        postProcessVolume.profile.TryGetSettings(out depthOfFieldEffect);

        // Set the material texture scale
        if (enemyRenderer != null)
        {
            enemyRenderer.material.mainTextureScale = materialScale;
        }
    }

    void Update()
    {
        float distanceToPlayer = Vector3.Distance(transform.position, player.position);

        // Release gas when player is in range
        if (distanceToPlayer <= gasRange && !isAttached)
        {
            if (!gasEffect.isPlaying) gasEffect.Play();
            StartCoroutine(ApplyPoisonEffect());
            EnableBlur();
        }
        else
        {
            gasEffect.Stop();
            StopCoroutine(ApplyPoisonEffect());
            DisableBlur();
        }

        // Attach to player if within close range
        if (distanceToPlayer <= attachRange && !isAttached)
        {
            StartCoroutine(AttachToPlayer());
        }
    }

    IEnumerator ApplyPoisonEffect()
    {
        while (true)
        {
            yield return new WaitForSeconds(poisonInterval);
            // Reduce player health here
            // e.g., playerHealth.TakeDamage(poisonDamage); or do i call damage script? I need to verify how team has this built.

        }
    }

    //void EnableBlur()
    //{
    //    // Enable blur post-processing effect
    //    postProcessVolume.weight = 1; // Adjust weight to your blur strength
    //}

    //void DisableBlur()
    //{
    //    // Disable blur post-processing effect
    //    postProcessVolume.weight = 0;
    //}

    void EnableBlur()
    {
        depthOfFieldEffect.active = true;
    }

    void DisableBlur()
    {
        depthOfFieldEffect.active = false;
    }

    IEnumerator AttachToPlayer()
    {
        isAttached = true;
        transform.SetParent(player);
        transform.localPosition = Vector3.zero; // Adjust attachment position
        yield return new WaitForSeconds(attachDuration);
        transform.SetParent(null);
        isAttached = false;
    }
}
