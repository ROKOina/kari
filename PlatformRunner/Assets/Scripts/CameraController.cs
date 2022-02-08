using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour
{
    [SerializeField]
    private Transform target;

    [SerializeField]
    private float smoothnedd = 0.1f;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if(target!=null)
        {
            //目標地点へ補完しながら移動
            transform.position = Vector3.Lerp(transform.position, target.position, smoothnedd);

            //斜め上にするために合成用の　Quaternion　を使う
            Quaternion q= Quaternion.Euler(0f, 30.0f, 0f);

            //キャラの向きをむく
            //プレイヤー右
            if (transform.rotation.y < target.rotation.y+q.y)
            {
                transform.Rotate(new Vector3(0, 1, 0), 0.5f);
            }
            //プレイヤー左
            if (transform.rotation.y > target.rotation.y - q.y)
            {
                transform.Rotate(new Vector3(0, 1, 0), -0.5f);
            }
        }
    }
}
