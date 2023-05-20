package dp.system.back.controllers;

import dp.system.back.exceptions.ReservationNotFoundException;
import dp.system.back.exceptions.UserNotFoundException;
import dp.system.back.models.ParkingLot;
import dp.system.back.services.UserService;
import org.json.JSONException;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.integration.annotation.ServiceActivator;
import org.springframework.integration.support.MessageBuilder;
import org.springframework.messaging.MessageChannel;
import org.springframework.messaging.MessageHandler;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RestController;

import java.util.ArrayList;
import java.util.List;

@Controller
@EnableScheduling
@RestController
public class MqttController {

    private final UserService userService;
    private final MessageChannel mqttOutputChannel;

    public MqttController(@Qualifier("mqttOutputChannel") MessageChannel mqttOutputChannel, UserService userService) {
        this.mqttOutputChannel = mqttOutputChannel;
        this.userService = userService;
    }

    @ServiceActivator(inputChannel = "mqttInputChannel")
    public MessageHandler handler() {
        return message -> {
            try {
                String messageType, plateNumber;

                JSONObject json = new JSONObject(message.getPayload().toString());
                messageType = json.getString("type");
                JSONObject json_ans = new JSONObject();
                switch (messageType) {
                    case "reset_request" -> {
                        userService.resetReservations();
                        json_ans.put("type", "reset_reply");
                        mqttOutputChannel.send(MessageBuilder.withPayload(json_ans.toString()).build());
                    }
                    case "enter_request" -> {
                        plateNumber = json.getString("plateNumber");
                        json_ans.put("type", "enter_reply");
                        json_ans.put("plateNumber", plateNumber);
                        try {
                            String reservedParkingSpaceNumber = userService.findReservationByPlateNumber(plateNumber);
                            json_ans.put("parkingSpaceNumber", reservedParkingSpaceNumber);
                            mqttOutputChannel.send(MessageBuilder.withPayload(json_ans.toString()).build());
                        } catch (UserNotFoundException e) {
                            json_ans.put("parkingSpaceNumber", "N");
                            mqttOutputChannel.send(MessageBuilder.withPayload(json_ans.toString()).build());
                        } catch (ReservationNotFoundException e) {
                            json_ans.put("parkingSpaceNumber", "Y");
                            mqttOutputChannel.send(MessageBuilder.withPayload(json_ans.toString()).build());
                        }
                    }
                    case "exit_request" -> {
                        plateNumber = json.getString("plateNumber");
                        userService.deleteReservationForPlateNumber(plateNumber);
                        json_ans.put("type", "exit_reply");
                        json_ans.put("plateNumber", plateNumber);
                        mqttOutputChannel.send(MessageBuilder.withPayload(json_ans.toString()).build());
                    }
                    case "led_state" -> {
                        List<Integer> values = transformToInt(json.getString("values"));
                        ParkingLot parkingLot = new ParkingLot(values);
                        System.out.println(parkingLot);
                    }
                    default -> {
                    }
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        };
    }

    private List<Integer> transformToInt(String input) {
        String[] elements = input.substring(1, input.length() - 1).split(",");

        List<Integer> arrayList = new ArrayList<>();
        for (String element : elements) {
            int value = Integer.parseInt(element.trim());
            arrayList.add(value);
        }
        return arrayList;
    }


}
