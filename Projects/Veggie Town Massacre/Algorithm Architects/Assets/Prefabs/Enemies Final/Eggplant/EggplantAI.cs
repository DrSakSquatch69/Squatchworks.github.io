using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;
using UnityEngine.UI;
using UnityEngine.Rendering.PostProcessing;

public class EggplantAI : MonoBehaviour, IDamage
{
    [Header("Enemy Settings")]
    [SerializeField] private Renderer model;
    [SerializeField] private Vector2 materialScale = new Vector2(1f, 1f);
    [SerializeField] private NavMeshAgent agent;
    [SerializeField] private GameObject enemyPrefab;
    [SerializeField] private int HP = 100;
    [SerializeField] private int maxRespawns = 0;
    [SerializeField] private float renderDistance = 50f;
    [SerializeField] private float gasRange = 5f;
    [SerializeField] private float attachRange = 1f;
    [SerializeField] private float poisonDamage = 1f;
    [SerializeField] private float poisonInterval = 1f;
    [SerializeField] private float attachDuration = 5f;

    [Header("Effects")]
    public ParticleSystem gasEffect;
    public PostProcessVolume postProcessVolume;
    public Slider enemyHpBar;
    private DepthOfField depthOfFieldEffect;

    private Transform player;
    private Renderer render;
    private Color colorOrig;
    private int hpOrig;
    private bool isPlayerInGasRange = false;

    // Tracking poison gas effect on the player
    private bool isPoisonGasActive = false;
    [SerializeField] private GameObject poisonGasPrefab;  // Poison gas prefab to clone
    private GameObject poisonGasInstance;                // Instance of the poison gas attached to the player

    private int currentRespawnCount = 0;  // Initialize it with 0

   void Start()
{
    // Check if the player is correctly assigned
    if (player == null)
    {
        player = GameObject.FindGameObjectWithTag("Player")?.transform;
        if (player == null)
        {
            Debug.LogWarning("Player not found! Ensure the player has the 'Player' tag.");
        }
    }

    render = GetComponent<Renderer>();
    colorOrig = model.material.color;
    hpOrig = HP;

    // Get the MeshFilter and MeshRenderer components
    MeshFilter meshFilter = GetComponent<MeshFilter>();
    MeshRenderer meshRenderer = GetComponent<MeshRenderer>();
    float scaleMultiplier = 3f;  // Adjust this value to change the size

    if (meshFilter != null)
    {
        Mesh mesh = meshFilter.mesh;
        if (mesh != null)
        {
            Vector3[] vertices = mesh.vertices;

            // Scale the vertices of the mesh
            for (int i = 0; i < vertices.Length; i++)
            {
                vertices[i] *= scaleMultiplier;
            }

            // Apply changes and recalculate the mesh properties
            mesh.vertices = vertices;
            mesh.RecalculateBounds();
            mesh.RecalculateNormals();
            mesh.RecalculateTangents();
        }
        else
        {
            Debug.LogError("Mesh is null. Ensure the MeshFilter has a valid mesh assigned.");
        }
    }
    else
    {
        Debug.LogError("MeshFilter component is missing on the GameObject.");
    }

    // Update the MeshRenderer properties
    if (meshRenderer != null)
    {
        // Optionally, update material properties if needed
        meshRenderer.material.mainTextureScale = new Vector2(materialScale.x / scaleMultiplier, materialScale.y / scaleMultiplier);
    }
    else
    {
        Debug.LogError("MeshRenderer component is missing on the GameObject.");
    }

    // Apply uniform scaling to the transform
    transform.localScale = new Vector3(scaleMultiplier, scaleMultiplier, scaleMultiplier);

    gameManager.instance.updateGameGoal(1);
    updateEnemyUI();

    // Ensure the gas effect is stopped when the enemy starts
    if (gasEffect != null && gasEffect.isPlaying)
    {
        gasEffect.Stop();
    }
}

    void Update()
    {
        updateEnemyUI();
        agent.SetDestination(player.position);

        float distanceToPlayer = Vector3.Distance(transform.position, player.position);

        // Poison Gas Effect - When the player is in range of the gas
        if (distanceToPlayer <= gasRange && !isPoisonGasActive)
        {
            if (!gasEffect.isPlaying) gasEffect.Play();
            if (!isPlayerInGasRange)
            {
                isPlayerInGasRange = true;
                StartCoroutine(ApplyPoisonDamage());
            }
        }
        else
        {
            if (isPlayerInGasRange)
            {
                isPlayerInGasRange = false;
                gasEffect.Stop();
                StopCoroutine(ApplyPoisonDamage());
            }
        }
    }

    IEnumerator ApplyPoisonDamage()
    {
        // Make sure we are not damaging the enemy itself
        if (player == null || player.CompareTag("Enemy"))
        {
            yield break;  // If the target is the enemy itself, exit the coroutine
        }

        // Proceed with applying damage to the player while gas is active
        while (isPlayerInGasRange || isPoisonGasActive) // Apply damage whether in range or attached
        {
            yield return new WaitForSeconds(poisonInterval);
            // Only apply damage to the player
            PlayerController playerController = player.GetComponent<PlayerController>();
            if (playerController != null)
            {
                playerController.takeDamage((int)poisonDamage, Vector3.zero, damageType.stationary);
            }
        }
    }

    // Method to manage poison gas attachment to the player
    void AttachPoisonGasToPlayer()
    {
        if (!isPoisonGasActive && player != null)
        {
            // Instantiate the poison gas prefab and attach it to the player
            poisonGasInstance = Instantiate(poisonGasPrefab, player.position, Quaternion.identity);
            poisonGasInstance.transform.SetParent(player);  // Attach poison gas to player
            isPoisonGasActive = true;

            // Start applying poison damage once attached
            StartCoroutine(ApplyPoisonDamage());

            // Destroy the poison gas after a certain duration
            StartCoroutine(DestroyPoisonGasAfterTime());
        }
    }

    IEnumerator DestroyPoisonGasAfterTime()
    {
        // Wait for a set amount of time (the duration of the poison effect)
        yield return new WaitForSeconds(attachDuration);

        // Destroy poison gas instance after duration
        if (poisonGasInstance != null)
        {
            Destroy(poisonGasInstance);
            isPoisonGasActive = false;
        }
    }

    public void takeDamage(int amount, Vector3 dir, damageType type)
    {
        HP -= amount;
        updateEnemyUI();
        StartCoroutine(flashColor());

        if (HP <= 0)
        {
            if (currentRespawnCount < maxRespawns)
            {
                Instantiate(enemyPrefab, transform.position + Vector3.right, Quaternion.identity).GetComponent<EggplantAI>().SetRespawnCount(currentRespawnCount + 1);
                Instantiate(enemyPrefab, transform.position + Vector3.left, Quaternion.identity).GetComponent<EggplantAI>().SetRespawnCount(currentRespawnCount + 1);
                gameManager.instance.updateGameGoal(+1);
            }
            else
            {
                gameManager.instance.updateGameGoal(-1);
            }
            Destroy(gameObject);
        }
    }

    public void updateEnemyUI()
    {
        float dist = Vector3.Distance(transform.position, player.position);
        if (dist <= renderDistance)
        {
            enemyHpBar.gameObject.SetActive(true);
            enemyHpBar.value = (float)HP / hpOrig;
            enemyHpBar.transform.rotation = Camera.main.transform.rotation;
        }
        else
        {
            enemyHpBar.gameObject.SetActive(false);
        }
    }

    public void SetRespawnCount(int respawnCount) => currentRespawnCount = respawnCount;

    IEnumerator flashColor()
    {
        model.material.color = Color.red;
        yield return new WaitForSeconds(0.1f);
        model.material.color = colorOrig;
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            isPlayerInGasRange = true;

            // Start the poison gas effect and enable the particle system
            if (!gasEffect.isPlaying)
            {
                gasEffect.Play();  // Play the gas effect when the player is inside the gas range
            }

            // Check if the player is close enough to the enemy to attach the gas
            float distanceToEnemy = Vector3.Distance(transform.position, player.position);
            if (distanceToEnemy <= attachRange && !isPoisonGasActive)
            {
                AttachPoisonGasToPlayer();
            }
        }
    }

    void OnTriggerExit(Collider other)
    {
        // Stop poison gas effect if the player exits range
        if (other.CompareTag("Player"))
        {
            isPlayerInGasRange = false;
            //Debug.Log("Player exited gas range");

            // Stop poison damage effect and disable particle system
            StopCoroutine(ApplyPoisonDamage());

            // Only stop the particle effect if it's playing
            if (gasEffect.isPlaying)
            {
                gasEffect.Stop();  // Stop the gas effect when the player exits the gas range
            }
        }
    }
}