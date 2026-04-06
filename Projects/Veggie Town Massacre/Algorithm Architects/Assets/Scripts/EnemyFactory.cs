//using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.PostProcessing;
//
public class EnemyFactory : MonoBehaviour
{
    
    public int currWave;
    public int maxWaves;
    public Wave[] waves;
    [SerializeField] float countdown;
    [SerializeField] Transform spawn1, spawn2, spawn3, spawn4, spawn5, spawn6, spawn7;

    // Start is called before the first frame update
    void Awake()
    {
        maxWaves = waves.Length - 1;
        currWave = 0;
        gameManager.instance.setCurrWave(currWave);
        gameManager.instance.setLastWave(currWave == maxWaves);
        StartCoroutine(SpawnWave());
    }

    // Update is called once per frame
    void Update()
    {
        countdown -= Time.deltaTime;

        if(gameManager.instance.GetEnemyCountCurrent() == 0 && countdown <= 0 && currWave < maxWaves)
        {
            currWave++;
            gameManager.instance.setCurrWave(currWave);
            gameManager.instance.setLastWave(currWave == maxWaves);
            StartCoroutine(SpawnWave());
        }

    }

    public IEnumerator SpawnWave()
    {
        for (int i = 0; i < waves[currWave].enemies.Length;)
        {
            int num = Random.Range(1, 7);

            if(num == 1)
            {
                Instantiate(waves[currWave].enemies[i], spawn1.position, Quaternion.identity);
            } else if (num == 2)
            {
                Instantiate(waves[currWave].enemies[i], spawn2.position, Quaternion.identity);
            }
            else if (num == 3)
            {
                Instantiate(waves[currWave].enemies[i], spawn3.position, Quaternion.identity);
            }
            else if (num == 4)
            {
                Instantiate(waves[currWave].enemies[i], spawn4.position, Quaternion.identity);
            }
            else if (num == 5)
            {
                Instantiate(waves[currWave].enemies[i], spawn5.position, Quaternion.identity);
            }
            else if (num == 6)
            {
                Instantiate(waves[currWave].enemies[i], spawn6.position, Quaternion.identity);
            }
            else if (num == 7)
            {
                Instantiate(waves[currWave].enemies[i], spawn7.position, Quaternion.identity);
            }

            yield return new WaitForSeconds(1);
            ++i;
        
        }
    }

    [System.Serializable]
    public class Wave
    {
        public GameObject[] enemies;
        public float nextEnemyTime;
    }
}
