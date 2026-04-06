using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.SceneManagement;

public class damage : MonoBehaviour
{
    public static damage instance;
    //
    public damageType type;
    [SerializeField] Rigidbody rb;

    public int damageAmount;
    [SerializeField] public int kingDamageAmountOriginal;
    [SerializeField] public int kingDamageAmount;
    [SerializeField] public int bulletSpeed;
    [SerializeField] int despawnTimer;
    [SerializeField] float butterSlowAmount;
    [SerializeField] float damageInterval;



    float playerSpeedChange;
    IDamage dmg;

    // Start is called before the first frame update
    void Start()
    {
        if (instance == null)
        {
            instance = this;
        }

        //else
        //{
        //    Destroy(this);
        //}

        if (type == damageType.bullet || type == damageType.butter || type == damageType.bouncing || type == damageType.fire || type == damageType.tomato || type == damageType.cabbage || type == damageType.king)
        {
            //rb.velocity = transform.forward * bulletSpeed;
            rb.velocity = (gameManager.instance.getPlayer().transform.position - transform.position) * bulletSpeed;
            Destroy(gameObject, despawnTimer);
        }

        else if (type == damageType.chaser)
        {
            Destroy(gameObject, despawnTimer);
        }

        playerSpeedChange = gameManager.instance.getOriginalPlayerSpeed();
        //kingDamageAmount = kingDamageAmountOriginal;
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.isTrigger)
        {
            return;
        }

        dmg = other.GetComponent<IDamage>();

        if (dmg != null && type != damageType.stationary && !other.CompareTag("Enemy"))
        {
            if (type == damageType.bullet || type == damageType.chaser || type == damageType.SplitBullet)
            {
                dmg.takeDamage(damageAmount, -(transform.position - other.transform.position).normalized * (damageAmount / 2), type);
            }

            if (type == damageType.king)
            {
                dmg.takeDamage(kingDamageAmount, -(transform.position - other.transform.position).normalized * (kingDamageAmount / 2), type);
            }

            if (type == damageType.butter)
            {
                //Marks off the player as getting buttered and then sets the speed change
                gameManager.instance.setIsButtered(true);
                playerSpeedChange /= butterSlowAmount;
                gameManager.instance.setPlayerSpeed(playerSpeedChange);
                playerSpeedChange = gameManager.instance.getOriginalPlayerSpeed();
            }

            //If enemy shoots player with fire bullet player is now on fire and calls the damage over time method
            if (type == damageType.fire)
            {
                gameManager.instance.setIsOnFire(true);

                gameManager.instance.playerScript.DoT();
            }

            if (type == damageType.tomato)
            {
                dmg.takeDamage(damageAmount, -(transform.position - other.transform.position).normalized * (damageAmount / 2), type);
                gameManager.instance.setIsTomatoed(true);
                gameManager.instance.TomatoSplat();
            }

            if (type == damageType.cabbage)
            {
                gameManager.instance.setIsCabbaged(true);

                gameManager.instance.playerScript.DoT();
            }

            if (type == damageType.toxic)
            {
                if (!gameManager.instance.playerScript.isInToxicGas)
                {
                    gameManager.instance.playerScript.EnterToxicGas();
                    StartCoroutine(gameManager.instance.playerScript.ToxicGasDoT());
                }
            }
        }

        if (type == damageType.stationary)
        {
            if (dmg != null)
            {
                InvokeRepeating("ApplyStationaryDamageFog", 0f, damageInterval);
            }
        }

        if (type == damageType.bouncing)
        {
            if (dmg == null)
            {
                if (other.CompareTag("Wall"))
                {
                    transform.forward = -transform.forward;
                }
                else
                {
                    transform.forward = -transform.forward;
                    rb.velocity = new Vector3(rb.velocity.x, bulletSpeed, rb.velocity.z);
                }

            }
            else
            {
                Destroy(gameObject);
            }
        }

        if (type == damageType.bullet || type == damageType.chaser || type == damageType.butter || type == damageType.fire || type == damageType.tomato || type == damageType.cabbage || type == damageType.king || type == damageType.SplitBullet)
        {
            Destroy(gameObject);
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (type == damageType.stationary)
        {
            CancelInvoke("ApplyStationaryDamageFog");
        }
    }

    private void ApplyStationaryDamageFog()
    {
        if (dmg != null)
        {
            dmg.takeDamage(damageAmount, Vector3.zero, type);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (type == damageType.chaser)
        {
            rb.velocity = (gameManager.instance.getPlayer().transform.position - transform.position).normalized * bulletSpeed * Time.deltaTime;
        }
    }
}




