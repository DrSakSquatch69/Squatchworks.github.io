using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SplitBullet : MonoBehaviour
{
    [Header("Bullet Settings")]
    [SerializeField] private GameObject bulletPrefab;
    [SerializeField] private float travelSpeed = 20f;
    [SerializeField] private float travelDistance = 10f;
    [SerializeField] private float despawnTime = 5f;
    [SerializeField] private int damageAmount = 10;
    [SerializeField] private bool canSplit = true;

    private Vector3 startPosition;
    private Rigidbody rb;
    private bool hasSplit = false;

    private void Start()
    {
        rb = GetComponent<Rigidbody>();
        startPosition = transform.position;

        // Set initial velocity for the bullet
        rb.velocity = transform.forward * travelSpeed;

        // Start the travel and split coroutine
        StartCoroutine(BulletTravelAndSplit());

        // Destroy the bullet after the despawn time
        Destroy(gameObject, despawnTime);
    }

    private IEnumerator BulletTravelAndSplit()
    {
        // Travel straight for the set distance
        while (Vector3.Distance(startPosition, transform.position) < travelDistance)
        {
            yield return null;
        }

        // If the bullet has already split or cannot split, exit
        if (hasSplit || !canSplit)
            yield break;

        hasSplit = true; // Mark the bullet as split

        // Destroy the original bullet
        Destroy(gameObject);

        // Directions for split bullets
        Vector3 middleDirection = transform.forward;
        Vector3 leftDirection = Quaternion.Euler(0, -22, 0) * middleDirection;
        Vector3 leftDirection2 = Quaternion.Euler(0, -45, 0) * middleDirection;
        Vector3 rightDirection = Quaternion.Euler(0, 22, 0) * middleDirection;
        Vector3 rightDirection2 = Quaternion.Euler(0, 45, 0) * middleDirection;

        // Instantiate the split bullets
        CreateSplitBullet(middleDirection);
        CreateSplitBullet(leftDirection);
        CreateSplitBullet(leftDirection2);
        CreateSplitBullet(rightDirection);
        CreateSplitBullet(rightDirection2);
    }

    private void CreateSplitBullet(Vector3 direction)
    {
        GameObject splitBullet = Instantiate(bulletPrefab, transform.position, Quaternion.identity);
        Rigidbody splitRb = splitBullet.GetComponent<Rigidbody>();

        if (splitRb != null)
        {
            splitRb.velocity = direction * travelSpeed;
        }

        // Set the damage amount for the split bullet
        damage damageComponent = splitBullet.GetComponent<damage>();
        if (damageComponent != null)
        {
            damageComponent.damageAmount = damageAmount;
            damageComponent.type = damageType.SplitBullet; 
        }

        // Destroy split bullet after the despawn timer
        Destroy(splitBullet, despawnTime);
    }
}