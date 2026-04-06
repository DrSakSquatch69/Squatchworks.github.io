using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;

using UnityEngine;
using UnityEngine.AI;
using UnityEngine.UI;

public class OrangeAI : MonoBehaviour, IDamage
{
    [SerializeField] Animator orangeAnimator;
    [SerializeField] public List<Renderer> Body;
    [SerializeField] Transform headPosition;
    [SerializeField] Transform shootPosition;
    [SerializeField] GameObject bullet;
    [SerializeField] float firerate;
    [SerializeField] int distancePlayerIsSeen;
    [SerializeField] int distanceOrangeCloses;
    [SerializeField] int rotateSpeed;
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

    bool isShooting;
    bool playerSighted;
    bool PlayerTooClose;
    bool isOpen;
    bool isClosed;
    bool canShoot;

    private OrangeWeakSpot WeakSpotScript;

    int currentRespawnCount = 1;
    //int activeEnemiesAI; //Used for tracking the active enemies 

    [SerializeField] Slider enemyHpBar;
    public bool isSliderOn;


    // Start is called before the first frame update
    void Start()
    {
        WeakSpotScript = gameObject.GetComponent<OrangeWeakSpot>();
        colorOrig = Body[0].material.color;
        hpOrig = HP;                                //set original hp
        render = GetComponent<Renderer>();        //getting the renderer of the game object
        gameManager.instance.updateGameGoal(1);

        ignoreMask = LayerMask.GetMask("Enemy");
        updateEnemyUI();
        PlayerDist();

    }
    void faceTarget()
    {
        Quaternion rotate = Quaternion.LookRotation(new Vector3(playerDirection.x, 0, playerDirection.z));
        transform.rotation = Quaternion.Lerp(transform.rotation, rotate, Time.deltaTime * rotateSpeed);
    }
    // Update is called once per frame
    void Update()
    {
        updateEnemyUI();
        // activeEnemiesAI = GameObject.FindGameObjectsWithTag("Enemy").Length; //Checks for the current amount of remaining active enemies
        PlayerDist();

        if (canSeePlayer())
        {

        }
    }

    private void PlayerDist()
    {
        float dist = Vector3.Distance(transform.position, gameManager.instance.getPlayer().transform.position);
        if (dist <= distancePlayerIsSeen && dist >= distanceOrangeCloses)
        {
            faceTarget();
            OpenUp();
            canShoot = true;
        }
        else
        {
            canShoot = false;
            CloseUp();
        }
    }
    public void Damage(int amount)
    {
        HP -= amount;
        if(HP <= 0)
        {
            gameManager.instance.updateGameGoal(-1);
            Destroy(gameObject);
        }
    }
    bool canSeePlayer()
    {
        playerDirection = gameManager.instance.getPlayer().transform.position - headPosition.position;
        RaycastHit hit;
        Debug.DrawRay(headPosition.position, playerDirection.normalized * distancePlayerIsSeen, Color.red);

        if (Physics.Raycast(headPosition.position, playerDirection, out hit, distancePlayerIsSeen, ~ignoreMask))
        {
            if (hit.collider.CompareTag("Player"))
            { 
            if (!isShooting) { Shoot(); }
                return true;
            }
        }
        return false;
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

    void OpenUp()
    {
       if(!isOpen)
        {
            orangeAnimator.SetBool("isOpen", true);
            orangeAnimator.SetBool("isClosed", false);
            isOpen = true;
            isClosed = false;
        }
    }

    void CloseUp()
    {
        if(!isClosed)
        {
            orangeAnimator.SetBool("isOpen", false );
            orangeAnimator.SetBool("isClosed", true );
            isClosed = true;
            isOpen = false;
        }
    }
    private void Shoot()
    {
        if(canShoot)
        {
            StartCoroutine(Shooting());
        }
    }
    
    public void takeDamage(int amount, Vector3 dir, damageType type)
    {
       
    }
    IEnumerator Shooting()
    {
        isShooting = true;
        Instantiate(bullet, shootPosition.position, transform.rotation);
        yield return new WaitForSeconds(firerate);
        isShooting = false;
    }
}
