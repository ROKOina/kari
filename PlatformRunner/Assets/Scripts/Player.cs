using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

[RequireComponent(typeof(CharacterController))]
[RequireComponent(typeof(Animator))]
public class Player : MonoBehaviour
{
    [SerializeField]
    private float maxMoveSpeed = 5.0f;  //最大移動速度

    private CharacterController controller;

    private float horizontalSpeed;  //水平移動速度

    [SerializeField]
    private float maxFallSpeed = 20.0f; //最大落下速度

    [SerializeField]
    private float gravity = 60.0f;  //重力

    private float verticalSpeed;    //垂直移動速度

    [SerializeField]
    private float jumpSpeed = 20.0f;    //ジャンプ速度

    [SerializeField]
    private float acceleration = 10.0f; //加速度

    [SerializeField]
    private float friction = 10.0f;  //摩擦係数

    [SerializeField]
    private float airControl = 0.3f;    //空中入力操作時の補正値

    private Animator animator;

    [SerializeField]
    private Text coinText;

    private int coinCount;  //獲得コイン枚数

    [SerializeField]
    private Transform spawnPoint;

    [SerializeField]
    private Text lifeText;

    [SerializeField]
    private int life = 5;   //ライフ数

    [SerializeField]
    private float pushPower = 1.0f;    //押す力

    private Transform activeFloor;  //踏んでいる床

    private Vector3 activeLocalFloorPoint;  //床を基準にしたときの相対位置

    private Vector3 activeGlobalFloorPoint; //世界を基準に考えた絶対位置

    private int airFrame;   //空中にいる時間（フレーム）

    // Start is called before the first frame update
    void Start()
    {
        controller = GetComponent<CharacterController>();
        animator = GetComponent<Animator>();
        Spawn();
        //UI表示更新
        UpdateCoinText();
        UpdateLifeText();
    }

    // Update is called once per frame
    void Update()
    {
        ////水平方向入力情報取得
        //float horizontal = Input.GetAxisRaw("Horizontal");

        ////移動処理
        //Vector3 move;
        //move.x = horizontal * maxMoveSpeed;
        //move.y = 0.0f;
        //move.z = 0.0f;
        //controller.Move(move * Time.deltaTime);

        //重力処理
        UpdateGravity();

        //ジャンプ処理
        UpdateJump();

        //進行方向更新処理
        UpdateDirection();

        //移動更新処理
        UpdateMovement();
    }

    //固定時間毎更新処理
    private void FixedUpdate()
    {
        //念のためZ軸位置が動かないように固定
        transform.position = new Vector3(transform.position.x, transform.position.y, 0);
    }

    //進行方向更新処理
    private void UpdateDirection()
    {
        //水平方向入力情報取得
        float horizontal = Input.GetAxisRaw("Horizontal");

        //加速処理
        float power = Mathf.Abs(horizontal);
        if(power>0.1f)
        {
            //水平軸入力情報から進行方向を設定
            Vector3 direction = new Vector3(horizontal, 0, 0);

            //進行方向を向くように回転設定
            transform.rotation = Quaternion.LookRotation(direction);

            //加速量を計算
            float speed = horizontal * acceleration * Time.deltaTime;

            //空中に浮いている時は移動量を補正する
            if(!controller.isGrounded)
            {
                speed *= airControl;
            }

            //加速処理
            horizontalSpeed += speed;

            //速度が一定以上なら制限する
            if(Mathf.Abs(horizontalSpeed)>maxMoveSpeed)
            {
                horizontalSpeed = Mathf.Sign(horizontalSpeed) * maxMoveSpeed;
            }
        }
        //減速処理
        else
        {
            if(Mathf.Abs(horizontalSpeed)>0)
            {
                //減速量を計算
                float speed = Mathf.Sign(horizontalSpeed) * friction * Time.deltaTime;

                //空中に浮いている時は移動量を補正する
                if (!controller.isGrounded)
                {
                    speed *= airControl;
                }

                //減速処理
                horizontalSpeed -= speed;

                //速度が一定以上なら止める
                if(Mathf.Abs(horizontalSpeed)<friction*Time.deltaTime)
                {
                    horizontalSpeed = 0.0f;
                }
            }
        }
        ////水平移動速度
        //horizontalSpeed = horizontal * maxMoveSpeed;

        //地面に接地している時
        if(controller.isGrounded)
        {
            //アニメーターにパラメーターを送信
            animator.SetFloat("Speed", power);
        }
    }

    //移動更新処理
    private void UpdateMovement()
    {
        //アニメーターにパラメーターを送信
        animator.SetBool("Ground", controller.isGrounded);

        //移動量を計算
        //Vector3 move = new Vector3(horizontalSpeed, 0, 0);
        Vector3 move = new Vector3(horizontalSpeed, verticalSpeed, 0);

        //床の移動に追従する処理
        if (activeFloor!=null)
        {
            //前回の床を基準にした位置と今回の床の位置から今回のグローバル基準の位置を算出
            Vector3 newGlobalFloorPoint = activeFloor.TransformPoint(activeLocalFloorPoint);
            //前回と今回のグローバル基準の位置の差分を求めることで床の移動量を算出する
            Vector3 moveDistance = newGlobalFloorPoint - activeGlobalFloorPoint;
            //現在の位置に床の移動量を加算する
            controller.enabled = false;
            transform.position += moveDistance;
            controller.enabled = true;
        }

        //空中時間更新
        airFrame++;
        if(airFrame>2)
        {
            activeFloor = null;
        }

        //移動処理（この関数内でOnControllerColliderHit関数が呼ばれる）
        controller.Move(move * Time.deltaTime);

        //床の移動量算出のため、床の基準にした位置を保存
        if(activeFloor!=null)
        {
            //グローバル基準の位置と床を基準にした位置を保存
            activeGlobalFloorPoint = transform.position;
            activeLocalFloorPoint = activeFloor.InverseTransformPoint(transform.position);
        }
    }

    //重力処理
    private void UpdateGravity()
    {
        //地面に接地している時の垂直移動速度は一定の重力量
        if(controller.isGrounded)
        {
            verticalSpeed = -gravity * Time.deltaTime;
        }
        //空中では垂直移動速度に重力を加算していく
        else
        {
            verticalSpeed -= gravity * Time.deltaTime;
        }
        //垂直移動速度が最大落下速度を超えないように制御
        verticalSpeed = Mathf.Max(verticalSpeed, -maxFallSpeed);
    }

    //ジャンプ処理
    private void UpdateJump()
    {
        //地面に接地している状態でジャンプボタンを押すと垂直移動速度を設定する
        if(controller.isGrounded)
        {
            if(Input.GetButtonDown("Jump"))
            {
                verticalSpeed = jumpSpeed;

                //アニメーターにパラメーターを送信
                animator.SetTrigger("Jump");
            }
        }
    }

    //コインUI表示更新
    private void UpdateCoinText()
    {
        coinText.text = "x" + coinCount;
    }

    //コイン獲得処理
    public void AddCoin(int amount)
    {
        coinCount += amount;

        //UI表示更新
        UpdateCoinText();
    }

    //スポーン
    private void Spawn()
    {
        //移動力をリセット
        verticalSpeed = 0.0f;
        horizontalSpeed = 0.0f;


        Warp(spawnPoint.position);
    }

    //ワープ
    public void Warp(Vector3 position)
    {
        // ※キャラクターコントローラー使用時に直接位置を変更する場合は
        //  　キャラクターコントローラーを無効化してから設定する必要がある
        controller.enabled = false;
        transform.position = position;
        controller.enabled = true;

    }

    //死亡処理
    public void Death()
    {
        //ライフを減らす
        life--;
        if (life <= 0)
        {
            //現在のシーンを再ロード
            SceneManager.LoadScene(SceneManager.GetActiveScene().name);
        }
        //UI表示更新
        UpdateLifeText();

        //スポーン
        Spawn();
    }

    private void UpdateLifeText()
    {
        lifeText.text = "x" + life;
    }

    //スポーンポイント設定
    public void SetSpawnPoint(Transform transform)
    {
        spawnPoint = transform;
    }

    //CharacterControllerの移動でコライダーと衝突した際に呼ばれる
    private void OnControllerColliderHit(ControllerColliderHit hit)
    {
        //剛体を押す
        PushRigidbody(hit);

        //天井チェック
        CeilingCheck(hit);

        //デストリガーとの衝突処理
        CollisionDeathTrigger(hit);

        //床チェック
        FloorCheck(hit);
    }

    //天井チェック
    private void CeilingCheck(ControllerColliderHit hit)
    {
        //当たった面の向きが下方向だったら天井
        if(hit.normal.y<-0.8f)
        {
            //上方向に力が働いている場合はリセット
            if(verticalSpeed>0.0f)
            {
                verticalSpeed = 0.0f;
            }
        }
    }

    //接触した剛体を押す
    private void PushRigidbody(ControllerColliderHit hit)
    {
        Rigidbody body = hit.collider.attachedRigidbody;

        //リジッドボディチェック
        if(body==null||body.isKinematic)
        {
            return;
        }
        //下方向には押さない
        if(hit.moveDirection.y<-0.3f)
        {
            return;
        }
        //移動方向から押す方向を算出する(上下方向には押さない)
        Vector3 pushDir = new Vector3(hit.moveDirection.x, 0, hit.moveDirection.z);
        Vector3 pushVelocity = pushDir * pushPower;
        pushVelocity.y = body.velocity.y;
        //押す力を設定
        body.velocity = pushVelocity;
    }

    //デストリガーとの衝突処理
    private void CollisionDeathTrigger(ControllerColliderHit hit)
    {
        //デストリガーと衝突したら死亡する
        DeathTrigger deathTrigger = hit.gameObject.GetComponent<DeathTrigger>();
        if(deathTrigger!=null)
        {
            Death();
        }
    }

    //床チェック
    private void FloorCheck(ControllerColliderHit hit)
    {
        //当たった面の向きが上方向だったら床
        if(hit.normal.y>0.9f)
        {
            //踏んでいる床を記憶
            activeFloor = hit.collider.transform;
            //空中時間をリセット
            airFrame = 0;
        }
    }

}
