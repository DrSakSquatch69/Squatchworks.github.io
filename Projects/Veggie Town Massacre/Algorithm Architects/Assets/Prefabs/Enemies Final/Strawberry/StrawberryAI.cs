using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;
using Unity.VisualScripting;
using UnityEngine.UI;

public class StrawberryAI : MonoBehaviour, IDamage
{
    

    float angleToPlayer;

    [SerializeField] Renderer model;
    [SerializeField] NavMeshAgent agent;
    [SerializeField] Transform headPosition;
    [SerializeField] Transform shootPosition;
    [SerializeField] GameObject bullet;
    [SerializeField] float firerate;
    //[SerializeField] int bulletDamage = 10;  // Amount of damage taken from player
    [SerializeField] int rotateSpeed;
    //[SerializeField] float spinSpeed = 360f; // Speed of spin in degrees per second
    [SerializeField] float burstDuration = 2f; // Duration of the burst
    //[SerializeField] float verticalAmplitude = 10f; // The range of up-and-down movement in degrees
    //[SerializeField] float verticalFrequency = 2f;  // How quickly the bullets move up and down
    [SerializeField] float randomMoveRange = 10f; // Range within which the enemy moves randomly

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
    //bool playerSighted;
    bool isMovingRandomly;

    int currentRespawnCount = 1;

    [SerializeField] Slider enemyHpBar;
    public bool isSliderOn;

    void Start()
    {
        colorOrig = model.material.color;
        hpOrig = HP;
        render = GetComponent<Renderer>();
        gameManager.instance.updateGameGoal(1);

        ignoreMask = LayerMask.GetMask("Enemy");
        updateEnemyUI();

        // Start random movement
        StartCoroutine(RandomMoveRoutine());
    }

    void Update()
    {
        updateEnemyUI();

        // Check if the player is within shooting range
        float distanceToPlayer = Vector3.Distance(transform.position, gameManager.instance.getPlayer().transform.position);

        // If the player is within a certain range, stop moving and shoot
        if (distanceToPlayer <= agent.stoppingDistance + 5f) // Adjust the range as needed
        {
            if (agent != null && agent.isActiveAndEnabled)
            {
                // Stop the enemy from moving
                agent.isStopped = true;
            }

            faceTarget();

            // Start shooting if not already shooting
            if (!isShooting)
            {
                StartCoroutine(SpinAndShoot());
            }
        }
        else
        {
            if (agent != null && agent.isActiveAndEnabled)
            {
                // Resume movement if the player is out of range
                agent.isStopped = false;
            }
        }
    }
    IEnumerator RandomMoveRoutine()
    {
        while (true)
        {
            if (!isShooting)
            {
                Vector3 randomDirection = Random.insideUnitSphere * randomMoveRange;  // Get random direction within specified range
                randomDirection += transform.position;  // Make sure the position is relative to the current location

                NavMeshHit hit;
                if (NavMesh.SamplePosition(randomDirection, out hit, randomMoveRange, NavMesh.AllAreas))
                {
                    agent.SetDestination(hit.position);  // Set the destination to the random point on the NavMesh
                }
            }

            // Wait for a random time before choosing the next random point
            yield return new WaitForSeconds(Random.Range(2f, 5f));
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
            if (hit.collider.CompareTag("Player"))
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
        // updateEnemyUI();

        StartCoroutine(flashColor());

        //when hp is zero or less, it destroys the object
        if (HP <= 0)
        {
            // --activeEnemiesAI;
            // gameManager.instance.ActiveCheck(activeEnemiesAI);

            // Check if enemy can respawn
            if (currentRespawnCount < maxRespawns)
            {


                //Creates two new enemies when this one dies
                GameObject enemy1 = Instantiate(enemyPrefab, transform.position + Vector3.right, Quaternion.identity); // offset position so theyre not stacked
                GameObject enemy2 = Instantiate(enemyPrefab, transform.position + Vector3.left, Quaternion.identity); // offset position so theyre not stacked

                // Set the respawn count of the new enemies to be 1 more than the current enemy
                enemy1.GetComponent<EnemyAI>().SetRespawnCount(currentRespawnCount + 1);
                enemy2.GetComponent<EnemyAI>().SetRespawnCount(currentRespawnCount + 1);

                //Increment the game goal by 1 for each new enemy
                gameManager.instance.updateGameGoal(+1);

            }
            else
            {
                // No more respawns allowed, decrement the game goal
                gameManager.instance.updateGameGoal(-1);

            }

            // Destroys current enemy
            Destroy(gameObject);

            //if (gameManager.instance.ActiveCheck(activeEnemiesAI))
            //{
            //    gameManager.instance.Waves();
            //}
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
           // playerSighted = true;
        }
    }

    void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            //playerSighted = false;
        }
    }

    IEnumerator SpinAndShoot()
    {
        isShooting = true;
        //float timeElapsed = 0f;
        int bulletCount = 50; // Adjust the number of bullets for the dome pattern
        float timeBetweenShots = burstDuration / bulletCount;

        for (int i = 0; i < bulletCount; i++)
        {
            // Generate a random direction for the bullet in a dome shape
            Vector3 shootDirection = GetRandomDomeDirection();

            // Instantiate the bullet and set its position
            GameObject bulletInstance = Instantiate(bullet, shootPosition.position, Quaternion.identity);

            // Apply velocity to the bullet in the calculated direction
            Rigidbody bulletRb = bulletInstance.GetComponent<Rigidbody>();
            if (bulletRb != null)
            {
                bulletRb.velocity = shootDirection * 20f; // Adjust the speed as needed
            }

            // Wait before shooting the next bullet
            yield return new WaitForSeconds(timeBetweenShots);
        }

        isShooting = false;
    }

    Vector3 GetRandomDomeDirection()
    {
        // Generate a random point on a hemisphere (dome) facing upward
        float theta = Random.Range(0f, Mathf.PI * 2); // Random angle around the y-axis
        float phi = Random.Range(0f, Mathf.PI / 2);   // Random angle from the upward direction

        // Calculate the direction using spherical coordinates
        float x = Mathf.Sin(phi) * Mathf.Cos(theta);
        float y = Mathf.Cos(phi); // Upward direction for dome shape
        float z = Mathf.Sin(phi) * Mathf.Sin(theta);

        return new Vector3(x, y, z).normalized;
    }
}
