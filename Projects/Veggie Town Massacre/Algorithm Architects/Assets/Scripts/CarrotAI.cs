using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;

using UnityEngine;
using UnityEngine.AI;
using UnityEngine.UI;
//
public class CarrotAI : MonoBehaviour, IDamage
{
    enum damageTypes { bullet, chaser, stationary, butter }

    [SerializeField] int viewAngle;
    float angleToPlayer;

    [SerializeField] Animator anim;
    [SerializeField] Renderer model;
    [SerializeField] NavMeshAgent agent;
    [SerializeField] Rigidbody rb;
    [SerializeField] Transform headPosition;
    [SerializeField] Transform shootPosition;
    [SerializeField] GameObject bullet;
    [SerializeField] float firerate;
    [SerializeField] int rotateSpeed;
    [SerializeField] float burrowedPos;
    [SerializeField] float burrowSpeed;

    int hpOrig;                                 //Original HP
    [SerializeField] int HP;
    [SerializeField] GameObject enemyPrefab;    //Refrence to the enemy prefab
    //[SerializeField] int maxRespawns = 0;       //limit the amound of respawns the enemy has

    [SerializeField] float minHPSize;
    [SerializeField] float maxHPSize;
    [SerializeField] float renderDistance;
    LayerMask ignoreMask;

    Color colorOrig;
    Vector3 playerDirection;
    GameObject playerObj;
    Renderer render;

    public bool isShooting;
    bool playerSighted;
    bool inGround;

    float origPos;
    [SerializeField] Slider enemyHpBar;
    public bool isSliderOn;


    // Start is called before the first frame update
    void Start()
    {
        //stores the original color
        playerObj = gameManager.instance.getPlayer();
        colorOrig = model.material.color;
        hpOrig = HP;                                //set original hp
        render = GetComponent<Renderer>();        //getting the renderer of the game object
        origPos = transform.position.y;
        inGround = true;
        //enemyHpBar = Instantiate(enemyHp, FindObjectOfType<Canvas>().transform).GetComponent<Image>();
        //enemyHpBar.transform.SetParent(gameManager.instance.enemyHpParent.transform);
        gameManager.instance.updateGameGoal(1);

        ignoreMask = LayerMask.GetMask("Enemy");
        updateEnemyUI();
    }

    // Update is called once per frame
    void Update()
    {
        updateEnemyUI();
        // activeEnemiesAI = GameObject.FindGameObjectsWithTag("Enemy").Length; //Checks for the current amount of remaining active enemies
        Vector3 PlayerPos = playerObj.transform.position;
        PlayerPos.y = 0;
        agent.SetDestination(PlayerPos);
        anim.SetFloat("Speed", agent.velocity.normalized.magnitude);
        
        if (playerSighted && canSeePlayer())
        {
        }

        burrow();
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
                    inGround = false;
                    burrow();
                    faceTarget();


                    if (!isShooting)
                    {
                        StartCoroutine(Shoot());
                    }
                } 
                else
                {
                    inGround = true;
                    burrow();
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
            gameManager.instance.updateGameGoal(-1);
            // Destroys current enemy
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
        } else
        {
            enemyHpBar.gameObject.SetActive(false);
            isSliderOn = false;
        }
        
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
        }
    }

    void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            playerSighted = false;
        }
    }

    IEnumerator Shoot()
    {
        isShooting = true;
        if (isShooting) { anim.SetTrigger("Shoot"); }
        Instantiate(bullet, shootPosition.position, transform.rotation);
        yield return new WaitForSeconds(firerate);
        isShooting = false;
        anim.ResetTrigger("Shoot");
    }

    void burrow()
    {
        if (inGround)
        {
            Vector3 burrow = new Vector3(transform.position.x, burrowedPos, transform.position.z);
            transform.position = Vector3.Lerp(transform.position, burrow, Time.deltaTime * burrowSpeed);
            //anim.SetBool("Burrowed", true);
        }
        else if (!inGround && agent.remainingDistance <= agent.stoppingDistance)
        {

            Vector3 resurface = new Vector3(transform.position.x, origPos, transform.position.z);
            transform.position = Vector3.Lerp(transform.position, resurface, Time.deltaTime * burrowSpeed);
            //anim.SetBool("Burrowed", false);
        }
    }
}
