using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;

using UnityEngine;
using UnityEngine.AI;
using UnityEngine.UI;
public class RadishAI : MonoBehaviour, IDamage
{
    enum damageTypes { bullet, chaser, stationary, butter }

    [SerializeField] int viewAngle;
    float angleToPlayer;

    [SerializeField] Renderer model;
    [SerializeField] NavMeshAgent agent;
    [SerializeField] Transform headPosition;
    [SerializeField] Transform shootPosition;
    [SerializeField] int rotateSpeed;
    [SerializeField] AudioClip explosionSound;
    [SerializeField] float explodingTime;
    [SerializeField] int explodingDamage;
    public ParticleSystem explodeEffect;

    int hpOrig;                                 //Original HP
    [SerializeField] int HP;
    [SerializeField] GameObject enemyPrefab;    //Refrence to the enemy prefab
    [SerializeField] int maxRespawns = 0;       //limit the amound of respawns the enemy has

    [SerializeField] float minHPSize;
    [SerializeField] float maxHPSize;
    [SerializeField] float renderDistance;
    LayerMask ignoreMask;

    Color colorOrig;
    Vector3 playerDirection;
    GameObject playerObj;
    Renderer render;

    bool isExploding;
    bool playerSighted;
    bool playerInRange;

    Collider playerCollider;

    int currentRespawnCount = 1;
    //int activeEnemiesAI; //Used for tracking the active enemies 

    [SerializeField] Slider enemyHpBar;
    [SerializeField] GameObject parent;
    public bool isSliderOn;


    // Start is called before the first frame update
    void Start()
    {
        //stores the original color
        playerObj = gameManager.instance.getPlayer();
        colorOrig = model.material.color;
        hpOrig = HP;                                //set original hp
        render = GetComponent<Renderer>();        //getting the renderer of the game object
        gameManager.instance.updateGameGoal(1);

        ignoreMask = LayerMask.GetMask("Enemy");
        updateEnemyUI();
    }

    // Update is called once per frame
    void Update()
    {
       updateEnemyUI();
        //activeEnemiesAI = GameObject.FindGameObjectsWithTag("Enemy").Length; //Checks for the current amount of remaining active enemies
        Vector3 PlayerPos = playerObj.transform.position;
        PlayerPos.y = 0;
        agent.SetDestination(PlayerPos);

        if (playerSighted && canSeePlayer())
        {

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

                if (!isExploding && playerInRange)
                {
                    StartCoroutine(Explode());
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

            }

            // Destroys current enemy
            if (!isExploding)
            {
                StartCoroutine(Explode());
            }

            //if (gameManager.instance.ActiveCheck(activeEnemiesAI))
            //{
            //    gameManager.instance.Waves();
            //}
        }
    }

    public void updateEnemyUI()
    {
        float dist = Vector3.Distance(transform.position, gameManager.instance.getPlayer().transform.position);  //get the distance between the player and enemy

        if (dist <= renderDistance && HP > 0)
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

    //Sends feedback to the user that they are doing damage
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
            playerInRange = true;
            playerCollider = other;
        }
    }

    void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            playerSighted = false;
            playerInRange = false;
        }
    }

    IEnumerator Explode()
    {
        isExploding = true;
        HP = 0;
        StartCoroutine(scaleUp());
        if (explosionSound != null)
        {
            gameManager.instance.playerScript.soundManager.playExplosion(explosionSound);
        }
        yield return new WaitForSeconds(explodingTime);
        gameManager.instance.cameraController.startShake(0.5f, 1.25f);
        if (playerInRange)
        {
            gameManager.instance.playerScript.takeDamage(explodingDamage, -(transform.position - playerCollider.transform.position).normalized * (explodingDamage * 2), damageType.melee);
        }
        if (explodeEffect != null)
        {
            Instantiate(explodeEffect, transform.position, Quaternion.identity);
        }
        gameManager.instance.updateGameGoal(-1);
        Destroy(parent);
        Destroy(gameObject);
    }

    IEnumerator scaleUp()
    {
        Vector3 scale = new Vector3(0.2f, 0.1f, 0.2f);
        transform.localScale = (transform.localScale + scale);
        yield return new WaitForSeconds(0.1f);
        StartCoroutine(scaleUp());
    }


}


