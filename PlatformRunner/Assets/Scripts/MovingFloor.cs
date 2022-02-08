using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MovingFloor : MonoBehaviour
{
    [SerializeField]
    private Vector3 move = new Vector3(-2.0f, 0.0f, 0.0f);  //移動量

    [SerializeField]
    private float time = 1.0f;  //元の位置から目的の位置を往復する時間

    private Vector3 originalPosition;   //元の位置

    private float theta;

    // Start is called before the first frame update
    void Start()
    {
        originalPosition = transform.position;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        //指定時間でθが1周する計算
        if(time!=0.0f)
        {
            theta += Mathf.PI * 2.0f * (1.0f / time) * Time.deltaTime;
        }
        //サインカーブを利用して-1.0～1.0の間を取得する
        float c = Mathf.Sin(theta);
        //-1.0～1.0の値を0.0～1.0の値に変更する
        c = (1.0f - c) * 0.5f;
        //元の位置から目的の位置までを往復する位置を算出する
        Vector3 position = Vector3.Lerp(originalPosition, originalPosition + move, c);
        transform.position = position;
    }
}
