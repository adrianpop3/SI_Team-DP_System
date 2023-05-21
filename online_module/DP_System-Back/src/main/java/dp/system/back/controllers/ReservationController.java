package dp.system.back.controllers;

import dp.system.back.models.Reservation;
import dp.system.back.models.User;
import org.json.JSONException;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.integration.support.MessageBuilder;
import org.springframework.messaging.MessageChannel;
import org.springframework.messaging.Message;
import org.springframework.stereotype.Component;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import org.springframework.messaging.PollableChannel;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.stereotype.Controller;

@RestController
@Controller
@EnableScheduling
@Component
public class ReservationController {

    private static int seq_number = 0;

    @Autowired
    private PollableChannel mqttInputChannel;

    @Autowired
    private MessageChannel mqttOutputChannel;

    @PostMapping("/reserve")
    public ResponseEntity<Reservation> reservationRequest(@RequestBody User userData) {
        seq_number++;
        int real_seq_nr = 0;
        Message<?> message;
        JSONObject json_ans = null;
        Reservation reservation = null;
        try {
            JSONObject json = new JSONObject();
            json.put("type", "reservation_request");
            json.put("seq_nr", seq_number);
            do {
                mqttOutputChannel.send(MessageBuilder.withPayload(json.toString()).build());
                message = mqttInputChannel.receive(5000);
                Thread.sleep(100);
                if (message != null) {
                    json_ans = new JSONObject(message.getPayload().toString());
                    if (json_ans.getString("type").equals("reservation_reply"))
                        real_seq_nr = json_ans.getInt("seq_nr");
                }
            } while (real_seq_nr != seq_number);
            if (!json_ans.getString("parkingSpaceNumber").equals("X")) {
                userData.setReservedParkingSpaceNumber(json_ans.getString("parkingSpaceNumber"));
                reservation = new Reservation(userData.getPlateNumber(), json_ans.getString("parkingSpaceNumber"));
            }
        } catch (JSONException | InterruptedException e) {
            e.printStackTrace();
        }
        return ResponseEntity.ok(reservation);
    }

}
