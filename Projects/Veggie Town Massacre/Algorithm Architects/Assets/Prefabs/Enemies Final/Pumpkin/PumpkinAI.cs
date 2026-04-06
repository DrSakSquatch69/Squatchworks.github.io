using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;

using UnityEngine;
using UnityEngine.AI;
using UnityEngine.UI;
//
public class PumpkinAI : MonoBehaviour, IDamage
{
    

    [SerializeField] int viewAngle;
    float angleToPlayer;

    [SerializeField] Renderer model;
    [SerializeField] NavMeshAgent agent;
    [SerializeField] Transform headPosition;
    [SerializeField] Transform shootPosition;
    [SerializeField] GameObject bullet;
    [SerializeField] float firerate = 10f; // Faster fire rate for high-speed shooting
    [SerializeField] float projectileSpeed = 30f; // Increased projectile speed
    [SerializeField] int rotateSpeed;
    [SerializeField] int maxBullets = 200; // Maximum bullets before cooldown
    [SerializeField] float cooldownDuration = 3f; // Cooldown time in seconds

    int hpOrig;
    [SerializeField] int HP;
    [SerializeField] GameObject enemyPrefab;
    [SerializeField] int maxRespawns = 0;

    [SerializeField] float minHPSize;
    [SerializeField] float maxHPSize;
    [SerializeField] float renderDistance;
    LayerMask ignoreMask;

    Color colorOrig;
    Vector3 playerDirection;
    GameObject playerObj;
    Renderer render;

    bool isShooting;
    bool playerSighted;
    bool isOnCooldown;

    int currentRespawnCount = 1;
    int bulletCount = 0; // Counter for bullets fired

    [SerializeField] Slider enemyHpBar;
    public bool isSliderOn;

    void Start()
    {
        playerObj = gameManager.instance.getPlayer();
        colorOrig = model.material.color;
        hpOrig = HP;
        render = GetComponent<Renderer>();
        gameManager.instance.updateGameGoal(1);

        ignoreMask = LayerMask.GetMask("Enemy");
        updateEnemyUI();
    }

    void Update()
    {
        updateEnemyUI();

        // Get the distance between the enemy and the player
        float distanceToPlayer = Vector3.Distance(transform.position, gameManager.instance.getPlayer().transform.position);

        // If the player is within the stopping distance, stop the agent
        if (distanceToPlayer <= agent.stoppingDistance)
        {
            agent.isStopped = true;
            faceTarget();
        }
        else
        {
            agent.isStopped = false;
            Vector3 PlayerPos = playerObj.transform.position;
            PlayerPos.y = 0;
            agent.SetDestination(PlayerPos);
        }

        // Engage the player if sighted and not on cooldown
        if (playerSighted && canSeePlayer() && !isOnCooldown)
        {
            if (!isShooting)
            {
                StartCoroutine(ShootRapidFire());
            }
        }
    }

    bool canSeePlayer()
    {
        playerDirection = gameManager.instance.getPlayer().transform.position - headPosition.position;
        angleToPlayer = Vector3.Angle(playerDirection, transform.forward);
        Debug.DrawRay(headPosition.position, playerDirection);

        RaycastHit hit;
        if (Physics.Raycast(headPosition.position, playerDirection, out hit, 1000, ~ignoreMask))
        {
            if (hit.collider.CompareTag("Player") && angleToPlayer <= viewAngle)
            {
                if (agent.remainingDistance <= agent.stoppingDistance)
                {
                    faceTarget();
                }
                return true;
            }
        }
        return false;
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
                GameObject enemy1 = Instantiate(enemyPrefab, transform.position + Vector3.right, Quaternion.identity);
                GameObject enemy2 = Instantiate(enemyPrefab, transform.position + Vector3.left, Quaternion.identity);
                enemy1.GetComponent<PumpkinAI>().SetRespawnCount(currentRespawnCount + 1);
                enemy2.GetComponent<PumpkinAI>().SetRespawnCount(currentRespawnCount + 1);
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
        float dist = Vector3.Distance(transform.position, gameManager.instance.getPlayer().transform.position);  //get the distance between the player and enemy

        if (dist <= renderDistance)
        {
            enemyHpBar.gameObject.SetActive(true);
            enemyHpBar.value = (float)HP / hpOrig;
            enemyHpBar.transform.rotation = Camera.main.transform.rotation;
            isSliderOn = true;
        }
        else
        {
            enemyHpBar.gameObject.SetActive(false);
            isSliderOn = false;
        }
    }

    public void SetRespawnCount(int respawnCount)
    {
        currentRespawnCount = respawnCount;
    }

    IEnumerator flashColor()
    {
        model.material.color = Color.red;
        yield return new WaitForSeconds(0.1f);
        model.material.color = colorOrig;
    }

    void faceTarget()
    {
        Quaternion rotate = Quaternion.LookRotation(new Vector3(playerDirection.x, 0, playerDirection.z));
        transform.rotation = Quaternion.Lerp(transform.rotation, rotate, Time.deltaTime * rotateSpeed);
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            playerSighted = true;
        }
    }

    void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            playerSighted = false;
        }
    }

    IEnumerator ShootRapidFire()
    {
        isShooting = true;
        while (playerSighted && bulletCount < maxBullets)
        {
            // Instantiate the bullet at the shootPosition's current position and rotation
            GameObject projectile = Instantiate(bullet, shootPosition.position, shootPosition.rotation);

            // Detach the bullet so it's not a child of the enemy
            projectile.transform.SetParent(null);

            // Get the Rigidbody component of the bullet
            Rigidbody rb = projectile.GetComponent<Rigidbody>();

            // Set the bullet's velocity using the shootPosition's forward direction
            rb.velocity = shootPosition.forward * projectileSpeed;

            // Ignore collision between the bullet and the enemy
            Collider bulletCollider = projectile.GetComponent<Collider>();
            Collider enemyCollider = GetComponent<Collider>();
            if (bulletCollider != null && enemyCollider != null)
            {
                Physics.IgnoreCollision(bulletCollider, enemyCollider);
            }

            bulletCount++;
            yield return new WaitForSeconds(firerate);
        }

        if (bulletCount >= maxBullets)
        {
            StartCoroutine(Cooldown());
        }

        isShooting = false;
    }

    IEnumerator Cooldown()
    {
        isOnCooldown = true;
        yield return new WaitForSeconds(cooldownDuration);
        bulletCount = 0; // Reset bullet counter
        isOnCooldown = false;
    }
}