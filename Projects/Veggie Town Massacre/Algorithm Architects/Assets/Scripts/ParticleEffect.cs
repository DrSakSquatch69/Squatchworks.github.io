using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ParticleEffect : MonoBehaviour
{
    [SerializeField] float dur;
    // Start is called before the first frame update
    void Start()
    {
        StartCoroutine(duration());
    }

    IEnumerator duration()
    {
        yield return new WaitForSeconds(dur);
        Destroy(gameObject);
    }
}
