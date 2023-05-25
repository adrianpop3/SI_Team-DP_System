package dp.system.back.controllers;

import dp.system.back.models.Reservation;
import dp.system.back.models.User;
import dp.system.back.repositories.UserRepository;
import org.json.JSONException;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.integration.support.MessageBuilder;
import org.springframework.messaging.Message;
import org.springframework.messaging.MessageChannel;
import org.springframework.stereotype.Component;
import org.springframework.web.bind.annotation.CrossOrigin;
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
@CrossOrigin(origins = "http://localhost:4200")
public class ReservationController {

    private static int seq_number = 0;
    private static boolean flag = false;

    @Autowired
    private UserRepository userRepository;

    @Autowired
    private PollableChannel mqttInputChannel;

    @Autowired
    private MessageChannel mqttOutputChannel;

    @PostMapping("/home/reserve")
    public ResponseEntity<Reservation> reservationRequest(@RequestBody User userData) {

        seq_number++;
        int real_seq_nr = 0;
        Message<?> message;
        JSONObject json_ans = null;
        Reservation reservation = null;

        User userFound = userRepository.findByPlateNumber(userData.getPlateNumber());
        if(userFound.getReservedParkingSpaceNumber() != null) {
            Reservation reservationFound = new Reservation(userData.getPlateNumber(), userFound.getReservedParkingSpaceNumber());
            return new ResponseEntity<>(reservationFound, HttpStatus.OK);
        }

        try {
            JSONObject json = new JSONObject();
            json.put("type", "reservation_request");
            json.put("seq_nr", seq_number);
            do {
                mqttOutputChannel.send(MessageBuilder.withPayload(json.toString()).build());
                message = mqttInputChannel.receive(500);
                Thread.sleep(100);
                if (message != null) {
                    json_ans = new JSONObject(message.getPayload().toString());
                    if (json_ans.getString("type").equals("reservation_reply"))
                        real_seq_nr = json_ans.getInt("seq_nr");

                    if (json_ans.getString("type").equals("reset_request")) {
                        flag = true;
                        break;
                    }
                }
            } while (real_seq_nr != seq_number);
            if (flag || json_ans.getString("parkingSpaceNumber").equals("X")) {
                reservation = new Reservation(userData.getPlateNumber(), "-");
                flag = false;
            } else if (!json_ans.getString("parkingSpaceNumber").equals("X")) {
                User user = userRepository.findByPlateNumber(userData.getPlateNumber());
                user.setReservedParkingSpaceNumber(json_ans.getString("parkingSpaceNumber"));
                reservation = new Reservation(userData.getPlateNumber(), json_ans.getString("parkingSpaceNumber"));
                userRepository.save(user);
            }
        } catch (JSONException | InterruptedException e) {
            e.printStackTrace();
        }
        return new ResponseEntity<>(reservation, HttpStatus.OK);
    }

}
