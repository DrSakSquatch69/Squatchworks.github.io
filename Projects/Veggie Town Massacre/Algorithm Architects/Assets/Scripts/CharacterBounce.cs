using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class CharacterBounce : MonoBehaviour
{
    [SerializeField] float bounceHeight = 0.2f; // Height of the bounce
    [SerializeField] float bounceSpeed = 20f;    // Speed of the bounce
    [SerializeField] NavMeshAgent agent;        // Reference to the NavMeshAgent

    float originalY;

    void Start()
    {
        originalY = transform.position.y;
    }

    void Update()
    {
        // Check if the character is walking (velocity is greater than a small threshold)
        if (agent.velocity.magnitude > 0.1f)
        {
            // Calculate the new Y position using a sine wave for bouncing effect
            float newY = originalY + Mathf.Sin(Time.time * bounceSpeed) * bounceHeight;
            transform.position = new Vector3(transform.position.x, newY, transform.position.z);
        }
        else
        {
            // Reset Y position when the character is not moving
            transform.position = new Vector3(transform.position.x, originalY, transform.position.z);
        }
    }
}