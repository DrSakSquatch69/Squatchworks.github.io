using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BouncePad : MonoBehaviour
{
    //bool playerInRange;
    [SerializeField] Animator anim;

    private void Update()
    {
        
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Player") && !gameManager.instance.isPaused)
        {
            gameManager.instance.playerScript.CheckForBouncePad();
            //playerInRange = true;

            //Place animation code here

            if (anim != null)
            {
                StartCoroutine(bounce());
            }
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Player") && !gameManager.instance.isPaused)
        {
            //playerInRange = false;
        }
    }

    IEnumerator bounce()
    {
        anim.SetTrigger("Bounce");
        yield return new WaitForSeconds(.5f);
        anim.ResetTrigger("Bounce");
    }
}
