using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;
//
public class ParticleTriggerHandler : MonoBehaviour
{
    [SerializeField] int damageAmount;
    [SerializeField] float damageInterval;
    [SerializeField] float detectionRadius;

    private List<IDamage> damageableObjects = new List<IDamage>();
    private ParticleSystem PS;
    private bool isInToxicCloud;
    private float nextDamageTime;
    private bool playerIsInToxicCloud;

    void Start()
    {
        PS = GetComponent<ParticleSystem>();
        InvokeRepeating("ApplyDamage", 0f, damageInterval);
    }

    void Update()
    {
        DetectDamageableObjects();
    }
    void DetectDamageableObjects()
    {
        Collider[] hitColliders = Physics.OverlapSphere(transform.position, detectionRadius);
        damageableObjects.Clear();

        foreach(var hitCollider in hitColliders)
        {
            IDamage damageable = hitCollider.GetComponent<IDamage>();
            
            if(hitCollider.CompareTag("Player")) {playerIsInToxicCloud = true; }
            else { playerIsInToxicCloud = false; }


            if(damageable != null && !damageableObjects.Contains(damageable)) 
            {
                damageableObjects.Add(damageable);
            }
        }
        if(damageableObjects.Count > 0)
        {
            isInToxicCloud = true;
        }
        else
        {
            isInToxicCloud = false;
        }
    }
    void OnParticleTrigger()
    {
        List<ParticleSystem.Particle> enter = new List<ParticleSystem.Particle>();
        List<ParticleSystem.Particle> exit = new List<ParticleSystem.Particle>();

        int numEnter = PS.GetTriggerParticles(ParticleSystemTriggerEventType.Enter, enter);
        int numExit = PS.GetTriggerParticles(ParticleSystemTriggerEventType.Exit, exit);

       // Debug.Log("Trigger Enter Count: " + numEnter);     //Debug Log
       // Debug.Log("Trigger Exit Count: " + numExit);        //Debug Log

        //Hanle entering particles
        while (numEnter > 0 && isInToxicCloud == false)
        {
            Debug.Log("Particles Entered");
            
            isInToxicCloud = true; 
        }

        //Handle exiting particles
        if (numExit > 0 && numEnter == 0)
        {
            Debug.Log("Particles Exited");
            isInToxicCloud = false;
        }
        PS.SetTriggerParticles(ParticleSystemTriggerEventType.Enter, enter);
        PS.SetTriggerParticles(ParticleSystemTriggerEventType.Exit, exit);
    }
    
    private void ApplyDamage()
    {
        if (isInToxicCloud)
        {
            foreach (var damageableObject in damageableObjects)
            {
                if (damageableObject != null)
                {
                    //Debug.Log("Applying Damage: " + damageAmount + "to" + damageableObject);  //debug log
                    damageableObject.takeDamage(damageAmount, Vector3.zero, damageType.stationary);
                    if(playerIsInToxicCloud) { gameManager.instance.GetSoundManager().PlayStationaryDMG(); }
                }
            }
        }
        if (!isInToxicCloud && SceneManager.GetActiveScene().buildIndex != 1 && SceneManager.GetActiveScene().buildIndex != 12)
            gameManager.instance.GetSoundManager().StopStationaryDMG();
    }
}